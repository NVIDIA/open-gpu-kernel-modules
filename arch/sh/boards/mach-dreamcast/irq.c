// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/boards/dreamcast/irq.c
 *
 * Holly IRQ support for the Sega Dreamcast.
 *
 * Copyright (c) 2001, 2002 M. R. Brown <mrbrown@0xd6.org>
 *
 * This file is part of the LinuxDC project (www.linuxdc.org)
 */
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/export.h>
#include <linux/err.h>
#include <mach/sysasic.h>

/*
 * Dreamcast System ASIC Hardware Events -
 *
 * The Dreamcast's System ASIC (a.k.a. Holly) is responsible for receiving
 * hardware events from system peripherals and triggering an SH7750 IRQ.
 * Hardware events can trigger IRQs 13, 11, or 9 depending on which bits are
 * set in the Event Mask Registers (EMRs).  When a hardware event is
 * triggered, its corresponding bit in the Event Status Registers (ESRs)
 * is set, and that bit should be rewritten to the ESR to acknowledge that
 * event.
 *
 * There are three 32-bit ESRs located at 0xa05f6900 - 0xa05f6908.  Event
 * types can be found in arch/sh/include/mach-dreamcast/mach/sysasic.h.
 * There are three groups of EMRs that parallel the ESRs.  Each EMR group
 * corresponds to an IRQ, so 0xa05f6910 - 0xa05f6918 triggers IRQ 13,
 * 0xa05f6920 - 0xa05f6928 triggers IRQ 11, and 0xa05f6930 - 0xa05f6938
 * triggers IRQ 9.
 *
 * In the kernel, these events are mapped to virtual IRQs so that drivers can
 * respond to them as they would a normal interrupt.  In order to keep this
 * mapping simple, the events are mapped as:
 *
 * 6900/6910 - Events  0-31, IRQ 13
 * 6904/6924 - Events 32-63, IRQ 11
 * 6908/6938 - Events 64-95, IRQ  9
 *
 */

#define ESR_BASE 0x005f6900    /* Base event status register */
#define EMR_BASE 0x005f6910    /* Base event mask register */

/*
 * Helps us determine the EMR group that this event belongs to: 0 = 0x6910,
 * 1 = 0x6920, 2 = 0x6930; also determine the event offset.
 */
#define LEVEL(event) (((event) - HW_EVENT_IRQ_BASE) / 32)

/* Return the hardware event's bit position within the EMR/ESR */
#define EVENT_BIT(event) (((event) - HW_EVENT_IRQ_BASE) & 31)

/*
 * For each of these *_irq routines, the IRQ passed in is the virtual IRQ
 * (logically mapped to the corresponding bit for the hardware event).
 */

/* Disable the hardware event by masking its bit in its EMR */
static inline void disable_systemasic_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;
	__u32 emr = EMR_BASE + (LEVEL(irq) << 4) + (LEVEL(irq) << 2);
	__u32 mask;

	mask = inl(emr);
	mask &= ~(1 << EVENT_BIT(irq));
	outl(mask, emr);
}

/* Enable the hardware event by setting its bit in its EMR */
static inline void enable_systemasic_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;
	__u32 emr = EMR_BASE + (LEVEL(irq) << 4) + (LEVEL(irq) << 2);
	__u32 mask;

	mask = inl(emr);
	mask |= (1 << EVENT_BIT(irq));
	outl(mask, emr);
}

/* Acknowledge a hardware event by writing its bit back to its ESR */
static void mask_ack_systemasic_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;
	__u32 esr = ESR_BASE + (LEVEL(irq) << 2);
	disable_systemasic_irq(data);
	outl((1 << EVENT_BIT(irq)), esr);
}

struct irq_chip systemasic_int = {
	.name		= "System ASIC",
	.irq_mask	= disable_systemasic_irq,
	.irq_mask_ack	= mask_ack_systemasic_irq,
	.irq_unmask	= enable_systemasic_irq,
};

/*
 * Map the hardware event indicated by the processor IRQ to a virtual IRQ.
 */
int systemasic_irq_demux(int irq)
{
	__u32 emr, esr, status, level;
	__u32 j, bit;

	switch (irq) {
	case 13:
		level = 0;
		break;
	case 11:
		level = 1;
		break;
	case  9:
		level = 2;
		break;
	default:
		return irq;
	}
	emr = EMR_BASE + (level << 4) + (level << 2);
	esr = ESR_BASE + (level << 2);

	/* Mask the ESR to filter any spurious, unwanted interrupts */
	status = inl(esr);
	status &= inl(emr);

	/* Now scan and find the first set bit as the event to map */
	for (bit = 1, j = 0; j < 32; bit <<= 1, j++) {
		if (status & bit) {
			irq = HW_EVENT_IRQ_BASE + j + (level << 5);
			return irq;
		}
	}

	/* Not reached */
	return irq;
}

void systemasic_irq_init(void)
{
	int irq_base, i;

	irq_base = irq_alloc_descs(HW_EVENT_IRQ_BASE, HW_EVENT_IRQ_BASE,
				   HW_EVENT_IRQ_MAX - HW_EVENT_IRQ_BASE, -1);
	if (IS_ERR_VALUE(irq_base)) {
		pr_err("%s: failed hooking irqs\n", __func__);
		return;
	}

	for (i = HW_EVENT_IRQ_BASE; i < HW_EVENT_IRQ_MAX; i++)
		irq_set_chip_and_handler(i, &systemasic_int, handle_level_irq);
}
