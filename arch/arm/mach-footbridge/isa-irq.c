// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/arch/arm/mach-footbridge/irq.c
 *
 *  Copyright (C) 1996-2000 Russell King
 *
 *  Changelog:
 *   22-Aug-1998 RMK	Restructured IRQ routines
 *   03-Sep-1998 PJB	Merged CATS support
 *   20-Jan-1998 RMK	Started merge of EBSA286, CATS and NetWinder
 *   26-Jan-1999 PJB	Don't use IACK on CATS
 *   16-Mar-1999 RMK	Added autodetect of ISA PICs
 */
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/spinlock.h>

#include <asm/mach/irq.h>

#include <mach/hardware.h>
#include <asm/hardware/dec21285.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include "common.h"

static void isa_mask_pic_lo_irq(struct irq_data *d)
{
	unsigned int mask = 1 << (d->irq & 7);

	outb(inb(PIC_MASK_LO) | mask, PIC_MASK_LO);
}

static void isa_ack_pic_lo_irq(struct irq_data *d)
{
	unsigned int mask = 1 << (d->irq & 7);

	outb(inb(PIC_MASK_LO) | mask, PIC_MASK_LO);
	outb(0x20, PIC_LO);
}

static void isa_unmask_pic_lo_irq(struct irq_data *d)
{
	unsigned int mask = 1 << (d->irq & 7);

	outb(inb(PIC_MASK_LO) & ~mask, PIC_MASK_LO);
}

static struct irq_chip isa_lo_chip = {
	.irq_ack	= isa_ack_pic_lo_irq,
	.irq_mask	= isa_mask_pic_lo_irq,
	.irq_unmask	= isa_unmask_pic_lo_irq,
};

static void isa_mask_pic_hi_irq(struct irq_data *d)
{
	unsigned int mask = 1 << (d->irq & 7);

	outb(inb(PIC_MASK_HI) | mask, PIC_MASK_HI);
}

static void isa_ack_pic_hi_irq(struct irq_data *d)
{
	unsigned int mask = 1 << (d->irq & 7);

	outb(inb(PIC_MASK_HI) | mask, PIC_MASK_HI);
	outb(0x62, PIC_LO);
	outb(0x20, PIC_HI);
}

static void isa_unmask_pic_hi_irq(struct irq_data *d)
{
	unsigned int mask = 1 << (d->irq & 7);

	outb(inb(PIC_MASK_HI) & ~mask, PIC_MASK_HI);
}

static struct irq_chip isa_hi_chip = {
	.irq_ack	= isa_ack_pic_hi_irq,
	.irq_mask	= isa_mask_pic_hi_irq,
	.irq_unmask	= isa_unmask_pic_hi_irq,
};

static void isa_irq_handler(struct irq_desc *desc)
{
	unsigned int isa_irq = *(unsigned char *)PCIIACK_BASE;

	if (isa_irq < _ISA_IRQ(0) || isa_irq >= _ISA_IRQ(16)) {
		do_bad_IRQ(desc);
		return;
	}

	generic_handle_irq(isa_irq);
}

static struct resource pic1_resource = {
	.name	= "pic1",
	.start	= 0x20,
	.end	= 0x3f,
};

static struct resource pic2_resource = {
	.name	= "pic2",
	.start	= 0xa0,
	.end	= 0xbf,
};

void __init isa_init_irq(unsigned int host_irq)
{
	unsigned int irq;

	/*
	 * Setup, and then probe for an ISA PIC
	 * If the PIC is not there, then we
	 * ignore the PIC.
	 */
	outb(0x11, PIC_LO);
	outb(_ISA_IRQ(0), PIC_MASK_LO);	/* IRQ number		*/
	outb(0x04, PIC_MASK_LO);	/* Slave on Ch2		*/
	outb(0x01, PIC_MASK_LO);	/* x86			*/
	outb(0xf5, PIC_MASK_LO);	/* pattern: 11110101	*/

	outb(0x11, PIC_HI);
	outb(_ISA_IRQ(8), PIC_MASK_HI);	/* IRQ number		*/
	outb(0x02, PIC_MASK_HI);	/* Slave on Ch1		*/
	outb(0x01, PIC_MASK_HI);	/* x86			*/
	outb(0xfa, PIC_MASK_HI);	/* pattern: 11111010	*/

	outb(0x0b, PIC_LO);
	outb(0x0b, PIC_HI);

	if (inb(PIC_MASK_LO) == 0xf5 && inb(PIC_MASK_HI) == 0xfa) {
		outb(0xff, PIC_MASK_LO);/* mask all IRQs	*/
		outb(0xff, PIC_MASK_HI);/* mask all IRQs	*/
	} else {
		printk(KERN_INFO "IRQ: ISA PIC not found\n");
		host_irq = (unsigned int)-1;
	}

	if (host_irq != (unsigned int)-1) {
		for (irq = _ISA_IRQ(0); irq < _ISA_IRQ(8); irq++) {
			irq_set_chip_and_handler(irq, &isa_lo_chip,
						 handle_level_irq);
			irq_clear_status_flags(irq, IRQ_NOREQUEST | IRQ_NOPROBE);
		}

		for (irq = _ISA_IRQ(8); irq < _ISA_IRQ(16); irq++) {
			irq_set_chip_and_handler(irq, &isa_hi_chip,
						 handle_level_irq);
			irq_clear_status_flags(irq, IRQ_NOREQUEST | IRQ_NOPROBE);
		}

		request_resource(&ioport_resource, &pic1_resource);
		request_resource(&ioport_resource, &pic2_resource);

		irq = IRQ_ISA_CASCADE;
		if (request_irq(irq, no_action, 0, "cascade", NULL))
			pr_err("Failed to request irq %u (cascade)\n", irq);

		irq_set_chained_handler(host_irq, isa_irq_handler);

		/*
		 * On the NetWinder, don't automatically
		 * enable ISA IRQ11 when it is requested.
		 * There appears to be a missing pull-up
		 * resistor on this line.
		 */
		if (machine_is_netwinder())
			irq_modify_status(_ISA_IRQ(11),
				IRQ_NOREQUEST | IRQ_NOPROBE, IRQ_NOAUTOEN);
	}
}


