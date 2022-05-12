/*
 * Allwinner A1X SoCs IRQ chip driver.
 *
 * Copyright (C) 2012 Maxime Ripard
 *
 * Maxime Ripard <maxime.ripard@free-electrons.com>
 *
 * Based on code from
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Benn Huang <benn@allwinnertech.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include <asm/exception.h>

#define SUN4I_IRQ_VECTOR_REG		0x00
#define SUN4I_IRQ_PROTECTION_REG	0x08
#define SUN4I_IRQ_NMI_CTRL_REG		0x0c
#define SUN4I_IRQ_PENDING_REG(x)	(0x10 + 0x4 * x)
#define SUN4I_IRQ_FIQ_PENDING_REG(x)	(0x20 + 0x4 * x)
#define SUN4I_IRQ_ENABLE_REG(data, x)	((data)->enable_reg_offset + 0x4 * x)
#define SUN4I_IRQ_MASK_REG(data, x)	((data)->mask_reg_offset + 0x4 * x)
#define SUN4I_IRQ_ENABLE_REG_OFFSET	0x40
#define SUN4I_IRQ_MASK_REG_OFFSET	0x50
#define SUNIV_IRQ_ENABLE_REG_OFFSET	0x20
#define SUNIV_IRQ_MASK_REG_OFFSET	0x30

struct sun4i_irq_chip_data {
	void __iomem *irq_base;
	struct irq_domain *irq_domain;
	u32 enable_reg_offset;
	u32 mask_reg_offset;
};

static struct sun4i_irq_chip_data *irq_ic_data;

static void __exception_irq_entry sun4i_handle_irq(struct pt_regs *regs);

static void sun4i_irq_ack(struct irq_data *irqd)
{
	unsigned int irq = irqd_to_hwirq(irqd);

	if (irq != 0)
		return; /* Only IRQ 0 / the ENMI needs to be acked */

	writel(BIT(0), irq_ic_data->irq_base + SUN4I_IRQ_PENDING_REG(0));
}

static void sun4i_irq_mask(struct irq_data *irqd)
{
	unsigned int irq = irqd_to_hwirq(irqd);
	unsigned int irq_off = irq % 32;
	int reg = irq / 32;
	u32 val;

	val = readl(irq_ic_data->irq_base +
			SUN4I_IRQ_ENABLE_REG(irq_ic_data, reg));
	writel(val & ~(1 << irq_off),
	       irq_ic_data->irq_base + SUN4I_IRQ_ENABLE_REG(irq_ic_data, reg));
}

static void sun4i_irq_unmask(struct irq_data *irqd)
{
	unsigned int irq = irqd_to_hwirq(irqd);
	unsigned int irq_off = irq % 32;
	int reg = irq / 32;
	u32 val;

	val = readl(irq_ic_data->irq_base +
			SUN4I_IRQ_ENABLE_REG(irq_ic_data, reg));
	writel(val | (1 << irq_off),
	       irq_ic_data->irq_base + SUN4I_IRQ_ENABLE_REG(irq_ic_data, reg));
}

static struct irq_chip sun4i_irq_chip = {
	.name		= "sun4i_irq",
	.irq_eoi	= sun4i_irq_ack,
	.irq_mask	= sun4i_irq_mask,
	.irq_unmask	= sun4i_irq_unmask,
	.flags		= IRQCHIP_EOI_THREADED | IRQCHIP_EOI_IF_HANDLED,
};

static int sun4i_irq_map(struct irq_domain *d, unsigned int virq,
			 irq_hw_number_t hw)
{
	irq_set_chip_and_handler(virq, &sun4i_irq_chip, handle_fasteoi_irq);
	irq_set_probe(virq);

	return 0;
}

static const struct irq_domain_ops sun4i_irq_ops = {
	.map = sun4i_irq_map,
	.xlate = irq_domain_xlate_onecell,
};

static int __init sun4i_of_init(struct device_node *node,
				struct device_node *parent)
{
	irq_ic_data->irq_base = of_iomap(node, 0);
	if (!irq_ic_data->irq_base)
		panic("%pOF: unable to map IC registers\n",
			node);

	/* Disable all interrupts */
	writel(0, irq_ic_data->irq_base + SUN4I_IRQ_ENABLE_REG(irq_ic_data, 0));
	writel(0, irq_ic_data->irq_base + SUN4I_IRQ_ENABLE_REG(irq_ic_data, 1));
	writel(0, irq_ic_data->irq_base + SUN4I_IRQ_ENABLE_REG(irq_ic_data, 2));

	/* Unmask all the interrupts, ENABLE_REG(x) is used for masking */
	writel(0, irq_ic_data->irq_base + SUN4I_IRQ_MASK_REG(irq_ic_data, 0));
	writel(0, irq_ic_data->irq_base + SUN4I_IRQ_MASK_REG(irq_ic_data, 1));
	writel(0, irq_ic_data->irq_base + SUN4I_IRQ_MASK_REG(irq_ic_data, 2));

	/* Clear all the pending interrupts */
	writel(0xffffffff, irq_ic_data->irq_base + SUN4I_IRQ_PENDING_REG(0));
	writel(0xffffffff, irq_ic_data->irq_base + SUN4I_IRQ_PENDING_REG(1));
	writel(0xffffffff, irq_ic_data->irq_base + SUN4I_IRQ_PENDING_REG(2));

	/* Enable protection mode */
	writel(0x01, irq_ic_data->irq_base + SUN4I_IRQ_PROTECTION_REG);

	/* Configure the external interrupt source type */
	writel(0x00, irq_ic_data->irq_base + SUN4I_IRQ_NMI_CTRL_REG);

	irq_ic_data->irq_domain = irq_domain_add_linear(node, 3 * 32,
						 &sun4i_irq_ops, NULL);
	if (!irq_ic_data->irq_domain)
		panic("%pOF: unable to create IRQ domain\n", node);

	set_handle_irq(sun4i_handle_irq);

	return 0;
}

static int __init sun4i_ic_of_init(struct device_node *node,
				   struct device_node *parent)
{
	irq_ic_data = kzalloc(sizeof(struct sun4i_irq_chip_data), GFP_KERNEL);
	if (!irq_ic_data) {
		pr_err("kzalloc failed!\n");
		return -ENOMEM;
	}

	irq_ic_data->enable_reg_offset = SUN4I_IRQ_ENABLE_REG_OFFSET;
	irq_ic_data->mask_reg_offset = SUN4I_IRQ_MASK_REG_OFFSET;

	return sun4i_of_init(node, parent);
}

IRQCHIP_DECLARE(allwinner_sun4i_ic, "allwinner,sun4i-a10-ic", sun4i_ic_of_init);

static int __init suniv_ic_of_init(struct device_node *node,
				   struct device_node *parent)
{
	irq_ic_data = kzalloc(sizeof(struct sun4i_irq_chip_data), GFP_KERNEL);
	if (!irq_ic_data) {
		pr_err("kzalloc failed!\n");
		return -ENOMEM;
	}

	irq_ic_data->enable_reg_offset = SUNIV_IRQ_ENABLE_REG_OFFSET;
	irq_ic_data->mask_reg_offset = SUNIV_IRQ_MASK_REG_OFFSET;

	return sun4i_of_init(node, parent);
}

IRQCHIP_DECLARE(allwinner_sunvi_ic, "allwinner,suniv-f1c100s-ic",
		suniv_ic_of_init);

static void __exception_irq_entry sun4i_handle_irq(struct pt_regs *regs)
{
	u32 hwirq;

	/*
	 * hwirq == 0 can mean one of 3 things:
	 * 1) no more irqs pending
	 * 2) irq 0 pending
	 * 3) spurious irq
	 * So if we immediately get a reading of 0, check the irq-pending reg
	 * to differentiate between 2 and 3. We only do this once to avoid
	 * the extra check in the common case of 1 happening after having
	 * read the vector-reg once.
	 */
	hwirq = readl(irq_ic_data->irq_base + SUN4I_IRQ_VECTOR_REG) >> 2;
	if (hwirq == 0 &&
		  !(readl(irq_ic_data->irq_base + SUN4I_IRQ_PENDING_REG(0)) &
			  BIT(0)))
		return;

	do {
		handle_domain_irq(irq_ic_data->irq_domain, hwirq, regs);
		hwirq = readl(irq_ic_data->irq_base +
				SUN4I_IRQ_VECTOR_REG) >> 2;
	} while (hwirq != 0);
}
