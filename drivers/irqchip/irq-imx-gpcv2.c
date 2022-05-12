// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 */

#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/slab.h>
#include <linux/irqchip.h>
#include <linux/syscore_ops.h>

#define IMR_NUM			4
#define GPC_MAX_IRQS            (IMR_NUM * 32)

#define GPC_IMR1_CORE0		0x30
#define GPC_IMR1_CORE1		0x40
#define GPC_IMR1_CORE2		0x1c0
#define GPC_IMR1_CORE3		0x1d0


struct gpcv2_irqchip_data {
	struct raw_spinlock	rlock;
	void __iomem		*gpc_base;
	u32			wakeup_sources[IMR_NUM];
	u32			saved_irq_mask[IMR_NUM];
	u32			cpu2wakeup;
};

static struct gpcv2_irqchip_data *imx_gpcv2_instance;

static void __iomem *gpcv2_idx_to_reg(struct gpcv2_irqchip_data *cd, int i)
{
	return cd->gpc_base + cd->cpu2wakeup + i * 4;
}

static int gpcv2_wakeup_source_save(void)
{
	struct gpcv2_irqchip_data *cd;
	void __iomem *reg;
	int i;

	cd = imx_gpcv2_instance;
	if (!cd)
		return 0;

	for (i = 0; i < IMR_NUM; i++) {
		reg = gpcv2_idx_to_reg(cd, i);
		cd->saved_irq_mask[i] = readl_relaxed(reg);
		writel_relaxed(cd->wakeup_sources[i], reg);
	}

	return 0;
}

static void gpcv2_wakeup_source_restore(void)
{
	struct gpcv2_irqchip_data *cd;
	int i;

	cd = imx_gpcv2_instance;
	if (!cd)
		return;

	for (i = 0; i < IMR_NUM; i++)
		writel_relaxed(cd->saved_irq_mask[i], gpcv2_idx_to_reg(cd, i));
}

static struct syscore_ops imx_gpcv2_syscore_ops = {
	.suspend	= gpcv2_wakeup_source_save,
	.resume		= gpcv2_wakeup_source_restore,
};

static int imx_gpcv2_irq_set_wake(struct irq_data *d, unsigned int on)
{
	struct gpcv2_irqchip_data *cd = d->chip_data;
	unsigned int idx = d->hwirq / 32;
	unsigned long flags;
	u32 mask, val;

	raw_spin_lock_irqsave(&cd->rlock, flags);
	mask = BIT(d->hwirq % 32);
	val = cd->wakeup_sources[idx];

	cd->wakeup_sources[idx] = on ? (val & ~mask) : (val | mask);
	raw_spin_unlock_irqrestore(&cd->rlock, flags);

	/*
	 * Do *not* call into the parent, as the GIC doesn't have any
	 * wake-up facility...
	 */

	return 0;
}

static void imx_gpcv2_irq_unmask(struct irq_data *d)
{
	struct gpcv2_irqchip_data *cd = d->chip_data;
	void __iomem *reg;
	u32 val;

	raw_spin_lock(&cd->rlock);
	reg = gpcv2_idx_to_reg(cd, d->hwirq / 32);
	val = readl_relaxed(reg);
	val &= ~BIT(d->hwirq % 32);
	writel_relaxed(val, reg);
	raw_spin_unlock(&cd->rlock);

	irq_chip_unmask_parent(d);
}

static void imx_gpcv2_irq_mask(struct irq_data *d)
{
	struct gpcv2_irqchip_data *cd = d->chip_data;
	void __iomem *reg;
	u32 val;

	raw_spin_lock(&cd->rlock);
	reg = gpcv2_idx_to_reg(cd, d->hwirq / 32);
	val = readl_relaxed(reg);
	val |= BIT(d->hwirq % 32);
	writel_relaxed(val, reg);
	raw_spin_unlock(&cd->rlock);

	irq_chip_mask_parent(d);
}

static struct irq_chip gpcv2_irqchip_data_chip = {
	.name			= "GPCv2",
	.irq_eoi		= irq_chip_eoi_parent,
	.irq_mask		= imx_gpcv2_irq_mask,
	.irq_unmask		= imx_gpcv2_irq_unmask,
	.irq_set_wake		= imx_gpcv2_irq_set_wake,
	.irq_retrigger		= irq_chip_retrigger_hierarchy,
	.irq_set_type		= irq_chip_set_type_parent,
#ifdef CONFIG_SMP
	.irq_set_affinity	= irq_chip_set_affinity_parent,
#endif
};

static int imx_gpcv2_domain_translate(struct irq_domain *d,
				      struct irq_fwspec *fwspec,
				      unsigned long *hwirq,
				      unsigned int *type)
{
	if (is_of_node(fwspec->fwnode)) {
		if (fwspec->param_count != 3)
			return -EINVAL;

		/* No PPI should point to this domain */
		if (fwspec->param[0] != 0)
			return -EINVAL;

		*hwirq = fwspec->param[1];
		*type = fwspec->param[2];
		return 0;
	}

	return -EINVAL;
}

static int imx_gpcv2_domain_alloc(struct irq_domain *domain,
				  unsigned int irq, unsigned int nr_irqs,
				  void *data)
{
	struct irq_fwspec *fwspec = data;
	struct irq_fwspec parent_fwspec;
	irq_hw_number_t hwirq;
	unsigned int type;
	int err;
	int i;

	err = imx_gpcv2_domain_translate(domain, fwspec, &hwirq, &type);
	if (err)
		return err;

	if (hwirq >= GPC_MAX_IRQS)
		return -EINVAL;

	for (i = 0; i < nr_irqs; i++) {
		irq_domain_set_hwirq_and_chip(domain, irq + i, hwirq + i,
				&gpcv2_irqchip_data_chip, domain->host_data);
	}

	parent_fwspec = *fwspec;
	parent_fwspec.fwnode = domain->parent->fwnode;
	return irq_domain_alloc_irqs_parent(domain, irq, nr_irqs,
					    &parent_fwspec);
}

static const struct irq_domain_ops gpcv2_irqchip_data_domain_ops = {
	.translate	= imx_gpcv2_domain_translate,
	.alloc		= imx_gpcv2_domain_alloc,
	.free		= irq_domain_free_irqs_common,
};

static const struct of_device_id gpcv2_of_match[] = {
	{ .compatible = "fsl,imx7d-gpc",  .data = (const void *) 2 },
	{ .compatible = "fsl,imx8mq-gpc", .data = (const void *) 4 },
	{ /* END */ }
};

static int __init imx_gpcv2_irqchip_init(struct device_node *node,
			       struct device_node *parent)
{
	struct irq_domain *parent_domain, *domain;
	struct gpcv2_irqchip_data *cd;
	const struct of_device_id *id;
	unsigned long core_num;
	int i;

	if (!parent) {
		pr_err("%pOF: no parent, giving up\n", node);
		return -ENODEV;
	}

	id = of_match_node(gpcv2_of_match, node);
	if (!id) {
		pr_err("%pOF: unknown compatibility string\n", node);
		return -ENODEV;
	}

	core_num = (unsigned long)id->data;

	parent_domain = irq_find_host(parent);
	if (!parent_domain) {
		pr_err("%pOF: unable to get parent domain\n", node);
		return -ENXIO;
	}

	cd = kzalloc(sizeof(struct gpcv2_irqchip_data), GFP_KERNEL);
	if (!cd) {
		pr_err("%pOF: kzalloc failed!\n", node);
		return -ENOMEM;
	}

	raw_spin_lock_init(&cd->rlock);

	cd->gpc_base = of_iomap(node, 0);
	if (!cd->gpc_base) {
		pr_err("%pOF: unable to map gpc registers\n", node);
		kfree(cd);
		return -ENOMEM;
	}

	domain = irq_domain_add_hierarchy(parent_domain, 0, GPC_MAX_IRQS,
				node, &gpcv2_irqchip_data_domain_ops, cd);
	if (!domain) {
		iounmap(cd->gpc_base);
		kfree(cd);
		return -ENOMEM;
	}
	irq_set_default_host(domain);

	/* Initially mask all interrupts */
	for (i = 0; i < IMR_NUM; i++) {
		void __iomem *reg = cd->gpc_base + i * 4;

		switch (core_num) {
		case 4:
			writel_relaxed(~0, reg + GPC_IMR1_CORE2);
			writel_relaxed(~0, reg + GPC_IMR1_CORE3);
			fallthrough;
		case 2:
			writel_relaxed(~0, reg + GPC_IMR1_CORE0);
			writel_relaxed(~0, reg + GPC_IMR1_CORE1);
		}
		cd->wakeup_sources[i] = ~0;
	}

	/* Let CORE0 as the default CPU to wake up by GPC */
	cd->cpu2wakeup = GPC_IMR1_CORE0;

	/*
	 * Due to hardware design failure, need to make sure GPR
	 * interrupt(#32) is unmasked during RUN mode to avoid entering
	 * DSM by mistake.
	 */
	writel_relaxed(~0x1, cd->gpc_base + cd->cpu2wakeup);

	imx_gpcv2_instance = cd;
	register_syscore_ops(&imx_gpcv2_syscore_ops);

	/*
	 * Clear the OF_POPULATED flag set in of_irq_init so that
	 * later the GPC power domain driver will not be skipped.
	 */
	of_node_clear_flag(node, OF_POPULATED);
	return 0;
}

IRQCHIP_DECLARE(imx_gpcv2_imx7d, "fsl,imx7d-gpc", imx_gpcv2_irqchip_init);
IRQCHIP_DECLARE(imx_gpcv2_imx8mq, "fsl,imx8mq-gpc", imx_gpcv2_irqchip_init);
