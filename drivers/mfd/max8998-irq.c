// SPDX-License-Identifier: GPL-2.0+
//
// Interrupt controller support for MAX8998
//
// Copyright (C) 2010 Samsung Electronics Co.Ltd
// Author: Joonyoung Shim <jy0922.shim@samsung.com>

#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/mfd/max8998-private.h>

struct max8998_irq_data {
	int reg;
	int mask;
};

static struct max8998_irq_data max8998_irqs[] = {
	[MAX8998_IRQ_DCINF] = {
		.reg = 1,
		.mask = MAX8998_IRQ_DCINF_MASK,
	},
	[MAX8998_IRQ_DCINR] = {
		.reg = 1,
		.mask = MAX8998_IRQ_DCINR_MASK,
	},
	[MAX8998_IRQ_JIGF] = {
		.reg = 1,
		.mask = MAX8998_IRQ_JIGF_MASK,
	},
	[MAX8998_IRQ_JIGR] = {
		.reg = 1,
		.mask = MAX8998_IRQ_JIGR_MASK,
	},
	[MAX8998_IRQ_PWRONF] = {
		.reg = 1,
		.mask = MAX8998_IRQ_PWRONF_MASK,
	},
	[MAX8998_IRQ_PWRONR] = {
		.reg = 1,
		.mask = MAX8998_IRQ_PWRONR_MASK,
	},
	[MAX8998_IRQ_WTSREVNT] = {
		.reg = 2,
		.mask = MAX8998_IRQ_WTSREVNT_MASK,
	},
	[MAX8998_IRQ_SMPLEVNT] = {
		.reg = 2,
		.mask = MAX8998_IRQ_SMPLEVNT_MASK,
	},
	[MAX8998_IRQ_ALARM1] = {
		.reg = 2,
		.mask = MAX8998_IRQ_ALARM1_MASK,
	},
	[MAX8998_IRQ_ALARM0] = {
		.reg = 2,
		.mask = MAX8998_IRQ_ALARM0_MASK,
	},
	[MAX8998_IRQ_ONKEY1S] = {
		.reg = 3,
		.mask = MAX8998_IRQ_ONKEY1S_MASK,
	},
	[MAX8998_IRQ_TOPOFFR] = {
		.reg = 3,
		.mask = MAX8998_IRQ_TOPOFFR_MASK,
	},
	[MAX8998_IRQ_DCINOVPR] = {
		.reg = 3,
		.mask = MAX8998_IRQ_DCINOVPR_MASK,
	},
	[MAX8998_IRQ_CHGRSTF] = {
		.reg = 3,
		.mask = MAX8998_IRQ_CHGRSTF_MASK,
	},
	[MAX8998_IRQ_DONER] = {
		.reg = 3,
		.mask = MAX8998_IRQ_DONER_MASK,
	},
	[MAX8998_IRQ_CHGFAULT] = {
		.reg = 3,
		.mask = MAX8998_IRQ_CHGFAULT_MASK,
	},
	[MAX8998_IRQ_LOBAT1] = {
		.reg = 4,
		.mask = MAX8998_IRQ_LOBAT1_MASK,
	},
	[MAX8998_IRQ_LOBAT2] = {
		.reg = 4,
		.mask = MAX8998_IRQ_LOBAT2_MASK,
	},
};

static inline struct max8998_irq_data *
irq_to_max8998_irq(struct max8998_dev *max8998, struct irq_data *data)
{
	return &max8998_irqs[data->hwirq];
}

static void max8998_irq_lock(struct irq_data *data)
{
	struct max8998_dev *max8998 = irq_data_get_irq_chip_data(data);

	mutex_lock(&max8998->irqlock);
}

static void max8998_irq_sync_unlock(struct irq_data *data)
{
	struct max8998_dev *max8998 = irq_data_get_irq_chip_data(data);
	int i;

	for (i = 0; i < ARRAY_SIZE(max8998->irq_masks_cur); i++) {
		/*
		 * If there's been a change in the mask write it back
		 * to the hardware.
		 */
		if (max8998->irq_masks_cur[i] != max8998->irq_masks_cache[i]) {
			max8998->irq_masks_cache[i] = max8998->irq_masks_cur[i];
			max8998_write_reg(max8998->i2c, MAX8998_REG_IRQM1 + i,
					max8998->irq_masks_cur[i]);
		}
	}

	mutex_unlock(&max8998->irqlock);
}

static void max8998_irq_unmask(struct irq_data *data)
{
	struct max8998_dev *max8998 = irq_data_get_irq_chip_data(data);
	struct max8998_irq_data *irq_data = irq_to_max8998_irq(max8998, data);

	max8998->irq_masks_cur[irq_data->reg - 1] &= ~irq_data->mask;
}

static void max8998_irq_mask(struct irq_data *data)
{
	struct max8998_dev *max8998 = irq_data_get_irq_chip_data(data);
	struct max8998_irq_data *irq_data = irq_to_max8998_irq(max8998, data);

	max8998->irq_masks_cur[irq_data->reg - 1] |= irq_data->mask;
}

static struct irq_chip max8998_irq_chip = {
	.name = "max8998",
	.irq_bus_lock = max8998_irq_lock,
	.irq_bus_sync_unlock = max8998_irq_sync_unlock,
	.irq_mask = max8998_irq_mask,
	.irq_unmask = max8998_irq_unmask,
};

static irqreturn_t max8998_irq_thread(int irq, void *data)
{
	struct max8998_dev *max8998 = data;
	u8 irq_reg[MAX8998_NUM_IRQ_REGS];
	int ret;
	int i;

	ret = max8998_bulk_read(max8998->i2c, MAX8998_REG_IRQ1,
			MAX8998_NUM_IRQ_REGS, irq_reg);
	if (ret < 0) {
		dev_err(max8998->dev, "Failed to read interrupt register: %d\n",
				ret);
		return IRQ_NONE;
	}

	/* Apply masking */
	for (i = 0; i < MAX8998_NUM_IRQ_REGS; i++)
		irq_reg[i] &= ~max8998->irq_masks_cur[i];

	/* Report */
	for (i = 0; i < MAX8998_IRQ_NR; i++) {
		if (irq_reg[max8998_irqs[i].reg - 1] & max8998_irqs[i].mask) {
			irq = irq_find_mapping(max8998->irq_domain, i);
			if (WARN_ON(!irq)) {
				disable_irq_nosync(max8998->irq);
				return IRQ_NONE;
			}
			handle_nested_irq(irq);
		}
	}

	return IRQ_HANDLED;
}

int max8998_irq_resume(struct max8998_dev *max8998)
{
	if (max8998->irq && max8998->irq_domain)
		max8998_irq_thread(max8998->irq, max8998);
	return 0;
}

static int max8998_irq_domain_map(struct irq_domain *d, unsigned int irq,
					irq_hw_number_t hw)
{
	struct max8997_dev *max8998 = d->host_data;

	irq_set_chip_data(irq, max8998);
	irq_set_chip_and_handler(irq, &max8998_irq_chip, handle_edge_irq);
	irq_set_nested_thread(irq, 1);
	irq_set_noprobe(irq);

	return 0;
}

static const struct irq_domain_ops max8998_irq_domain_ops = {
	.map = max8998_irq_domain_map,
};

int max8998_irq_init(struct max8998_dev *max8998)
{
	int i;
	int ret;
	struct irq_domain *domain;

	if (!max8998->irq) {
		dev_warn(max8998->dev,
			 "No interrupt specified, no interrupts\n");
		return 0;
	}

	mutex_init(&max8998->irqlock);

	/* Mask the individual interrupt sources */
	for (i = 0; i < MAX8998_NUM_IRQ_REGS; i++) {
		max8998->irq_masks_cur[i] = 0xff;
		max8998->irq_masks_cache[i] = 0xff;
		max8998_write_reg(max8998->i2c, MAX8998_REG_IRQM1 + i, 0xff);
	}

	max8998_write_reg(max8998->i2c, MAX8998_REG_STATUSM1, 0xff);
	max8998_write_reg(max8998->i2c, MAX8998_REG_STATUSM2, 0xff);

	domain = irq_domain_add_simple(NULL, MAX8998_IRQ_NR,
			max8998->irq_base, &max8998_irq_domain_ops, max8998);
	if (!domain) {
		dev_err(max8998->dev, "could not create irq domain\n");
		return -ENODEV;
	}
	max8998->irq_domain = domain;

	ret = request_threaded_irq(max8998->irq, NULL, max8998_irq_thread,
				   IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
				   "max8998-irq", max8998);
	if (ret) {
		dev_err(max8998->dev, "Failed to request IRQ %d: %d\n",
			max8998->irq, ret);
		return ret;
	}

	if (!max8998->ono)
		return 0;

	ret = request_threaded_irq(max8998->ono, NULL, max8998_irq_thread,
				   IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING |
				   IRQF_ONESHOT, "max8998-ono", max8998);
	if (ret)
		dev_err(max8998->dev, "Failed to request IRQ %d: %d\n",
			max8998->ono, ret);

	return 0;
}

void max8998_irq_exit(struct max8998_dev *max8998)
{
	if (max8998->ono)
		free_irq(max8998->ono, max8998);

	if (max8998->irq)
		free_irq(max8998->irq, max8998);
}
