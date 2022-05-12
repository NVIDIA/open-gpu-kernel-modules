// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 * Copyright (c) 2004 MIPS Inc
 * Author: chris@mips.com
 *
 * Copyright (C) 2004, 06 Ralf Baechle <ralf@linux-mips.org>
 */
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kernel_stat.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/msc01_ic.h>
#include <asm/traps.h>

static unsigned long _icctrl_msc;
#define MSC01_IC_REG_BASE	_icctrl_msc

#define MSCIC_WRITE(reg, data)	do { *(volatile u32 *)(reg) = data; } while (0)
#define MSCIC_READ(reg, data)	do { data = *(volatile u32 *)(reg); } while (0)

static unsigned int irq_base;

/* mask off an interrupt */
static inline void mask_msc_irq(struct irq_data *d)
{
	unsigned int irq = d->irq;

	if (irq < (irq_base + 32))
		MSCIC_WRITE(MSC01_IC_DISL, 1<<(irq - irq_base));
	else
		MSCIC_WRITE(MSC01_IC_DISH, 1<<(irq - irq_base - 32));
}

/* unmask an interrupt */
static inline void unmask_msc_irq(struct irq_data *d)
{
	unsigned int irq = d->irq;

	if (irq < (irq_base + 32))
		MSCIC_WRITE(MSC01_IC_ENAL, 1<<(irq - irq_base));
	else
		MSCIC_WRITE(MSC01_IC_ENAH, 1<<(irq - irq_base - 32));
}

/*
 * Masks and ACKs an IRQ
 */
static void level_mask_and_ack_msc_irq(struct irq_data *d)
{
	mask_msc_irq(d);
	if (!cpu_has_veic)
		MSCIC_WRITE(MSC01_IC_EOI, 0);
}

/*
 * Masks and ACKs an IRQ
 */
static void edge_mask_and_ack_msc_irq(struct irq_data *d)
{
	unsigned int irq = d->irq;

	mask_msc_irq(d);
	if (!cpu_has_veic)
		MSCIC_WRITE(MSC01_IC_EOI, 0);
	else {
		u32 r;
		MSCIC_READ(MSC01_IC_SUP+irq*8, r);
		MSCIC_WRITE(MSC01_IC_SUP+irq*8, r | ~MSC01_IC_SUP_EDGE_BIT);
		MSCIC_WRITE(MSC01_IC_SUP+irq*8, r);
	}
}

/*
 * Interrupt handler for interrupts coming from SOC-it.
 */
void ll_msc_irq(void)
{
	unsigned int irq;

	/* read the interrupt vector register */
	MSCIC_READ(MSC01_IC_VEC, irq);
	if (irq < 64)
		do_IRQ(irq + irq_base);
	else {
		/* Ignore spurious interrupt */
	}
}

static void msc_bind_eic_interrupt(int irq, int set)
{
	MSCIC_WRITE(MSC01_IC_RAMW,
		    (irq<<MSC01_IC_RAMW_ADDR_SHF) | (set<<MSC01_IC_RAMW_DATA_SHF));
}

static struct irq_chip msc_levelirq_type = {
	.name = "SOC-it-Level",
	.irq_ack = level_mask_and_ack_msc_irq,
	.irq_mask = mask_msc_irq,
	.irq_mask_ack = level_mask_and_ack_msc_irq,
	.irq_unmask = unmask_msc_irq,
	.irq_eoi = unmask_msc_irq,
};

static struct irq_chip msc_edgeirq_type = {
	.name = "SOC-it-Edge",
	.irq_ack = edge_mask_and_ack_msc_irq,
	.irq_mask = mask_msc_irq,
	.irq_mask_ack = edge_mask_and_ack_msc_irq,
	.irq_unmask = unmask_msc_irq,
	.irq_eoi = unmask_msc_irq,
};


void __init init_msc_irqs(unsigned long icubase, unsigned int irqbase, msc_irqmap_t *imp, int nirq)
{
	_icctrl_msc = (unsigned long) ioremap(icubase, 0x40000);

	/* Reset interrupt controller - initialises all registers to 0 */
	MSCIC_WRITE(MSC01_IC_RST, MSC01_IC_RST_RST_BIT);

	board_bind_eic_interrupt = &msc_bind_eic_interrupt;

	for (; nirq > 0; nirq--, imp++) {
		int n = imp->im_irq;

		switch (imp->im_type) {
		case MSC01_IRQ_EDGE:
			irq_set_chip_and_handler_name(irqbase + n,
						      &msc_edgeirq_type,
						      handle_edge_irq,
						      "edge");
			if (cpu_has_veic)
				MSCIC_WRITE(MSC01_IC_SUP+n*8, MSC01_IC_SUP_EDGE_BIT);
			else
				MSCIC_WRITE(MSC01_IC_SUP+n*8, MSC01_IC_SUP_EDGE_BIT | imp->im_lvl);
			break;
		case MSC01_IRQ_LEVEL:
			irq_set_chip_and_handler_name(irqbase + n,
						      &msc_levelirq_type,
						      handle_level_irq,
						      "level");
			if (cpu_has_veic)
				MSCIC_WRITE(MSC01_IC_SUP+n*8, 0);
			else
				MSCIC_WRITE(MSC01_IC_SUP+n*8, imp->im_lvl);
		}
	}

	irq_base = irqbase;

	MSCIC_WRITE(MSC01_IC_GENA, MSC01_IC_GENA_GENA_BIT);	/* Enable interrupt generation */

}
