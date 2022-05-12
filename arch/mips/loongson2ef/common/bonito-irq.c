// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 * Copyright (C) 2000, 2001 Ralf Baechle (ralf@gnu.org)
 *
 * Copyright (C) 2007 Lemote Inc. & Institute of Computing Technology
 * Author: Fuxin Zhang, zhangfx@lemote.com
 */
#include <linux/interrupt.h>
#include <linux/compiler.h>

#include <loongson.h>

static inline void bonito_irq_enable(struct irq_data *d)
{
	LOONGSON_INTENSET = (1 << (d->irq - LOONGSON_IRQ_BASE));
	mmiowb();
}

static inline void bonito_irq_disable(struct irq_data *d)
{
	LOONGSON_INTENCLR = (1 << (d->irq - LOONGSON_IRQ_BASE));
	mmiowb();
}

static struct irq_chip bonito_irq_type = {
	.name		= "bonito_irq",
	.irq_mask	= bonito_irq_disable,
	.irq_unmask	= bonito_irq_enable,
};

void bonito_irq_init(void)
{
	u32 i;

	for (i = LOONGSON_IRQ_BASE; i < LOONGSON_IRQ_BASE + 32; i++)
		irq_set_chip_and_handler(i, &bonito_irq_type,
					 handle_level_irq);

#ifdef CONFIG_CPU_LOONGSON2E
	i = LOONGSON_IRQ_BASE + 10;
	if (request_irq(i, no_action, 0, "dma_timeout", NULL))
		pr_err("Failed to request irq %d (dma_timeout)\n", i);
#endif
}
