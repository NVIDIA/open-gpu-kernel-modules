// SPDX-License-Identifier: GPL-2.0-only
/*
 * arch/arm/mach-ixp4xx/ixdp425-pci.c
 *
 * IXDP425 board-level PCI initialization
 *
 * Copyright (C) 2002 Intel Corporation.
 * Copyright (C) 2003-2004 MontaVista Software, Inc.
 *
 * Maintainer: Deepak Saxena <dsaxena@plexity.net>
 */

#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <asm/mach/pci.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>

#include "irqs.h"

#define MAX_DEV		4
#define IRQ_LINES	4

/* PCI controller GPIO to IRQ pin mappings */
#define INTA		11
#define INTB		10
#define INTC		9
#define INTD		8


void __init ixdp425_pci_preinit(void)
{
	irq_set_irq_type(IXP4XX_GPIO_IRQ(INTA), IRQ_TYPE_LEVEL_LOW);
	irq_set_irq_type(IXP4XX_GPIO_IRQ(INTB), IRQ_TYPE_LEVEL_LOW);
	irq_set_irq_type(IXP4XX_GPIO_IRQ(INTC), IRQ_TYPE_LEVEL_LOW);
	irq_set_irq_type(IXP4XX_GPIO_IRQ(INTD), IRQ_TYPE_LEVEL_LOW);
	ixp4xx_pci_preinit();
}

static int __init ixdp425_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	static int pci_irq_table[IRQ_LINES] = {
		IXP4XX_GPIO_IRQ(INTA),
		IXP4XX_GPIO_IRQ(INTB),
		IXP4XX_GPIO_IRQ(INTC),
		IXP4XX_GPIO_IRQ(INTD)
	};

	if (slot >= 1 && slot <= MAX_DEV && pin >= 1 && pin <= IRQ_LINES)
		return pci_irq_table[(slot + pin - 2) % 4];

	return -1;
}

struct hw_pci ixdp425_pci __initdata = {
	.nr_controllers = 1,
	.ops		= &ixp4xx_ops,
	.preinit	= ixdp425_pci_preinit,
	.setup		= ixp4xx_setup,
	.map_irq	= ixdp425_map_irq,
};

int __init ixdp425_pci_init(void)
{
	if (machine_is_ixdp425() || machine_is_ixcdp1100() ||
			machine_is_ixdp465() || machine_is_kixrp435())
		pci_common_init(&ixdp425_pci);
	return 0;
}

subsys_initcall(ixdp425_pci_init);
