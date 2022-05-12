// SPDX-License-Identifier: GPL-2.0-only
/*
 * arch/arch/mach-ixp4xx/fsg-pci.c
 *
 * FSG board-level PCI initialization
 *
 * Author: Rod Whitby <rod@whitby.id.au>
 * Maintainer: http://www.nslu2-linux.org/
 *
 * based on ixdp425-pci.c:
 *	Copyright (C) 2002 Intel Corporation.
 *	Copyright (C) 2003-2004 MontaVista Software, Inc.
 */

#include <linux/pci.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <asm/mach/pci.h>
#include <asm/mach-types.h>

#include "irqs.h"

#define MAX_DEV		3
#define IRQ_LINES	3

/* PCI controller GPIO to IRQ pin mappings */
#define INTA	6
#define INTB	7
#define INTC	5

void __init fsg_pci_preinit(void)
{
	irq_set_irq_type(IXP4XX_GPIO_IRQ(INTA), IRQ_TYPE_LEVEL_LOW);
	irq_set_irq_type(IXP4XX_GPIO_IRQ(INTB), IRQ_TYPE_LEVEL_LOW);
	irq_set_irq_type(IXP4XX_GPIO_IRQ(INTC), IRQ_TYPE_LEVEL_LOW);
	ixp4xx_pci_preinit();
}

static int __init fsg_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	static int pci_irq_table[IRQ_LINES] = {
		IXP4XX_GPIO_IRQ(INTC),
		IXP4XX_GPIO_IRQ(INTB),
		IXP4XX_GPIO_IRQ(INTA),
	};

	int irq = -1;
	slot -= 11;

	if (slot >= 1 && slot <= MAX_DEV && pin >= 1 && pin <= IRQ_LINES)
		irq = pci_irq_table[slot - 1];
	printk(KERN_INFO "%s: Mapped slot %d pin %d to IRQ %d\n",
	       __func__, slot, pin, irq);

	return irq;
}

struct hw_pci fsg_pci __initdata = {
	.nr_controllers = 1,
	.ops		= &ixp4xx_ops,
	.preinit =	  fsg_pci_preinit,
	.setup =	  ixp4xx_setup,
	.map_irq =	  fsg_map_irq,
};

int __init fsg_pci_init(void)
{
	if (machine_is_fsg())
		pci_common_init(&fsg_pci);
	return 0;
}

subsys_initcall(fsg_pci_init);
