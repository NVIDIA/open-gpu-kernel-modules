/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 */
#ifndef __ASM_MACH_ATH79_IRQ_H
#define __ASM_MACH_ATH79_IRQ_H

#define MIPS_CPU_IRQ_BASE	0
#define NR_IRQS			51

#define ATH79_CPU_IRQ(_x)	(MIPS_CPU_IRQ_BASE + (_x))

#define ATH79_MISC_IRQ_BASE	8
#define ATH79_MISC_IRQ_COUNT	32
#define ATH79_MISC_IRQ(_x)	(ATH79_MISC_IRQ_BASE + (_x))

#define ATH79_PCI_IRQ_BASE	(ATH79_MISC_IRQ_BASE + ATH79_MISC_IRQ_COUNT)
#define ATH79_PCI_IRQ_COUNT	6
#define ATH79_PCI_IRQ(_x)	(ATH79_PCI_IRQ_BASE + (_x))

#define ATH79_IP2_IRQ_BASE	(ATH79_PCI_IRQ_BASE + ATH79_PCI_IRQ_COUNT)
#define ATH79_IP2_IRQ_COUNT	2
#define ATH79_IP2_IRQ(_x)	(ATH79_IP2_IRQ_BASE + (_x))

#define ATH79_IP3_IRQ_BASE	(ATH79_IP2_IRQ_BASE + ATH79_IP2_IRQ_COUNT)
#define ATH79_IP3_IRQ_COUNT     3
#define ATH79_IP3_IRQ(_x)       (ATH79_IP3_IRQ_BASE + (_x))

#include <asm/mach-generic/irq.h>

#endif /* __ASM_MACH_ATH79_IRQ_H */
