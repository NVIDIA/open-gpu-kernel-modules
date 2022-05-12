/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __HARDWARE_H
#define __HARDWARE_H

#include <asm/types.h>

/*
 * Note about PCI IO space mappings
 *
 * To make IO space accesses efficient, we store virtual addresses in
 * the IO resources.
 *
 * The PCI IO space is located at virtual 0xfe000000 from physical
 * 0x90000000. The PCI BARs must be programmed with physical addresses,
 * but when we read them, we convert them to virtual addresses. See
 * arch/arm/plat-iop/pci.c.
 */

#ifndef __ASSEMBLY__
void iop32x_init_irq(void);
#endif


/*
 * Generic chipset bits
 */
#include "iop3xx.h"

/*
 * Board specific bits
 */
#include "glantank.h"
#include "iq80321.h"
#include "iq31244.h"
#include "n2100.h"


#endif
