/*
 * Code to handle IP32 IRQs
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2000 Harald Koerfgen
 * Copyright (C) 2001 Keith M Wesolowski
 */
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/random.h>
#include <linux/sched.h>
#include <linux/sched/debug.h>

#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <asm/signal.h>
#include <asm/time.h>
#include <asm/ip32/crime.h>
#include <asm/ip32/mace.h>
#include <asm/ip32/ip32_ints.h>

/* issue a PIO read to make sure no PIO writes are pending */
static inline void flush_crime_bus(void)
{
	crime->control;
}

static inline void flush_mace_bus(void)
{
	mace->perif.ctrl.misc;
}

/*
 * O2 irq map
 *
 * IP0 -> software (ignored)
 * IP1 -> software (ignored)
 * IP2 -> (irq0) C crime 1.1 all interrupts; crime 1.5 ???
 * IP3 -> (irq1) X unknown
 * IP4 -> (irq2) X unknown
 * IP5 -> (irq3) X unknown
 * IP6 -> (irq4) X unknown
 * IP7 -> (irq5) 7 CPU count/compare timer (system timer)
 *
 * crime: (C)
 *
 * CRIME_INT_STAT 31:0:
 *
 * 0  ->  8  Video in 1
 * 1  ->  9 Video in 2
 * 2  -> 10  Video out
 * 3  -> 11  Mace ethernet
 * 4  -> S  SuperIO sub-interrupt
 * 5  -> M  Miscellaneous sub-interrupt
 * 6  -> A  Audio sub-interrupt
 * 7  -> 15  PCI bridge errors
 * 8  -> 16  PCI SCSI aic7xxx 0
 * 9  -> 17 PCI SCSI aic7xxx 1
 * 10 -> 18 PCI slot 0
 * 11 -> 19 unused (PCI slot 1)
 * 12 -> 20 unused (PCI slot 2)
 * 13 -> 21 unused (PCI shared 0)
 * 14 -> 22 unused (PCI shared 1)
 * 15 -> 23 unused (PCI shared 2)
 * 16 -> 24 GBE0 (E)
 * 17 -> 25 GBE1 (E)
 * 18 -> 26 GBE2 (E)
 * 19 -> 27 GBE3 (E)
 * 20 -> 28 CPU errors
 * 21 -> 29 Memory errors
 * 22 -> 30 RE empty edge (E)
 * 23 -> 31 RE full edge (E)
 * 24 -> 32 RE idle edge (E)
 * 25 -> 33 RE empty level
 * 26 -> 34 RE full level
 * 27 -> 35 RE idle level
 * 28 -> 36 unused (software 0) (E)
 * 29 -> 37 unused (software 1) (E)
 * 30 -> 38 unused (software 2) - crime 1.5 CPU SysCorError (E)
 * 31 -> 39 VICE
 *
 * S, M, A: Use the MACE ISA interrupt register
 * MACE_ISA_INT_STAT 31:0
 *
 * 0-7 -> 40-47 Audio
 * 8 -> 48 RTC
 * 9 -> 49 Keyboard
 * 10 -> X Keyboard polled
 * 11 -> 51 Mouse
 * 12 -> X Mouse polled
 * 13-15 -> 53-55 Count/compare timers
 * 16-19 -> 56-59 Parallel (16 E)
 * 20-25 -> 60-62 Serial 1 (22 E)
 * 26-31 -> 66-71 Serial 2 (28 E)
 *
 * Note that this means IRQs 12-14, 50, and 52 do not exist.  This is a
 * different IRQ map than IRIX uses, but that's OK as Linux irq handling
 * is quite different anyway.
 */

/* Some initial interrupts to set up */
extern irqreturn_t crime_memerr_intr(int irq, void *dev_id);
extern irqreturn_t crime_cpuerr_intr(int irq, void *dev_id);

/*
 * This is for pure CRIME interrupts - ie not MACE.  The advantage?
 * We get to split the register in half and do faster lookups.
 */

static uint64_t crime_mask;

static inline void crime_enable_irq(struct irq_data *d)
{
	unsigned int bit = d->irq - CRIME_IRQ_BASE;

	crime_mask |= 1 << bit;
	crime->imask = crime_mask;
}

static inline void crime_disable_irq(struct irq_data *d)
{
	unsigned int bit = d->irq - CRIME_IRQ_BASE;

	crime_mask &= ~(1 << bit);
	crime->imask = crime_mask;
	flush_crime_bus();
}

static struct irq_chip crime_level_interrupt = {
	.name		= "IP32 CRIME",
	.irq_mask	= crime_disable_irq,
	.irq_unmask	= crime_enable_irq,
};

static void crime_edge_mask_and_ack_irq(struct irq_data *d)
{
	unsigned int bit = d->irq - CRIME_IRQ_BASE;
	uint64_t crime_int;

	/* Edge triggered interrupts must be cleared. */
	crime_int = crime->hard_int;
	crime_int &= ~(1 << bit);
	crime->hard_int = crime_int;

	crime_disable_irq(d);
}

static struct irq_chip crime_edge_interrupt = {
	.name		= "IP32 CRIME",
	.irq_ack	= crime_edge_mask_and_ack_irq,
	.irq_mask	= crime_disable_irq,
	.irq_mask_ack	= crime_edge_mask_and_ack_irq,
	.irq_unmask	= crime_enable_irq,
};

/*
 * This is for MACE PCI interrupts.  We can decrease bus traffic by masking
 * as close to the source as possible.	This also means we can take the
 * next chunk of the CRIME register in one piece.
 */

static unsigned long macepci_mask;

static void enable_macepci_irq(struct irq_data *d)
{
	macepci_mask |= MACEPCI_CONTROL_INT(d->irq - MACEPCI_SCSI0_IRQ);
	mace->pci.control = macepci_mask;
	crime_mask |= 1 << (d->irq - CRIME_IRQ_BASE);
	crime->imask = crime_mask;
}

static void disable_macepci_irq(struct irq_data *d)
{
	crime_mask &= ~(1 << (d->irq - CRIME_IRQ_BASE));
	crime->imask = crime_mask;
	flush_crime_bus();
	macepci_mask &= ~MACEPCI_CONTROL_INT(d->irq - MACEPCI_SCSI0_IRQ);
	mace->pci.control = macepci_mask;
	flush_mace_bus();
}

static struct irq_chip ip32_macepci_interrupt = {
	.name = "IP32 MACE PCI",
	.irq_mask = disable_macepci_irq,
	.irq_unmask = enable_macepci_irq,
};

/* This is used for MACE ISA interrupts.  That means bits 4-6 in the
 * CRIME register.
 */

#define MACEISA_AUDIO_INT	(MACEISA_AUDIO_SW_INT |		\
				 MACEISA_AUDIO_SC_INT |		\
				 MACEISA_AUDIO1_DMAT_INT |	\
				 MACEISA_AUDIO1_OF_INT |	\
				 MACEISA_AUDIO2_DMAT_INT |	\
				 MACEISA_AUDIO2_MERR_INT |	\
				 MACEISA_AUDIO3_DMAT_INT |	\
				 MACEISA_AUDIO3_MERR_INT)
#define MACEISA_MISC_INT	(MACEISA_RTC_INT |		\
				 MACEISA_KEYB_INT |		\
				 MACEISA_KEYB_POLL_INT |	\
				 MACEISA_MOUSE_INT |		\
				 MACEISA_MOUSE_POLL_INT |	\
				 MACEISA_TIMER0_INT |		\
				 MACEISA_TIMER1_INT |		\
				 MACEISA_TIMER2_INT)
#define MACEISA_SUPERIO_INT	(MACEISA_PARALLEL_INT |		\
				 MACEISA_PAR_CTXA_INT |		\
				 MACEISA_PAR_CTXB_INT |		\
				 MACEISA_PAR_MERR_INT |		\
				 MACEISA_SERIAL1_INT |		\
				 MACEISA_SERIAL1_TDMAT_INT |	\
				 MACEISA_SERIAL1_TDMAPR_INT |	\
				 MACEISA_SERIAL1_TDMAME_INT |	\
				 MACEISA_SERIAL1_RDMAT_INT |	\
				 MACEISA_SERIAL1_RDMAOR_INT |	\
				 MACEISA_SERIAL2_INT |		\
				 MACEISA_SERIAL2_TDMAT_INT |	\
				 MACEISA_SERIAL2_TDMAPR_INT |	\
				 MACEISA_SERIAL2_TDMAME_INT |	\
				 MACEISA_SERIAL2_RDMAT_INT |	\
				 MACEISA_SERIAL2_RDMAOR_INT)

static unsigned long maceisa_mask;

static void enable_maceisa_irq(struct irq_data *d)
{
	unsigned int crime_int = 0;

	pr_debug("maceisa enable: %u\n", d->irq);

	switch (d->irq) {
	case MACEISA_AUDIO_SW_IRQ ... MACEISA_AUDIO3_MERR_IRQ:
		crime_int = MACE_AUDIO_INT;
		break;
	case MACEISA_RTC_IRQ ... MACEISA_TIMER2_IRQ:
		crime_int = MACE_MISC_INT;
		break;
	case MACEISA_PARALLEL_IRQ ... MACEISA_SERIAL2_RDMAOR_IRQ:
		crime_int = MACE_SUPERIO_INT;
		break;
	}
	pr_debug("crime_int %08x enabled\n", crime_int);
	crime_mask |= crime_int;
	crime->imask = crime_mask;
	maceisa_mask |= 1 << (d->irq - MACEISA_AUDIO_SW_IRQ);
	mace->perif.ctrl.imask = maceisa_mask;
}

static void disable_maceisa_irq(struct irq_data *d)
{
	unsigned int crime_int = 0;

	maceisa_mask &= ~(1 << (d->irq - MACEISA_AUDIO_SW_IRQ));
	if (!(maceisa_mask & MACEISA_AUDIO_INT))
		crime_int |= MACE_AUDIO_INT;
	if (!(maceisa_mask & MACEISA_MISC_INT))
		crime_int |= MACE_MISC_INT;
	if (!(maceisa_mask & MACEISA_SUPERIO_INT))
		crime_int |= MACE_SUPERIO_INT;
	crime_mask &= ~crime_int;
	crime->imask = crime_mask;
	flush_crime_bus();
	mace->perif.ctrl.imask = maceisa_mask;
	flush_mace_bus();
}

static void mask_and_ack_maceisa_irq(struct irq_data *d)
{
	unsigned long mace_int;

	/* edge triggered */
	mace_int = mace->perif.ctrl.istat;
	mace_int &= ~(1 << (d->irq - MACEISA_AUDIO_SW_IRQ));
	mace->perif.ctrl.istat = mace_int;

	disable_maceisa_irq(d);
}

static struct irq_chip ip32_maceisa_level_interrupt = {
	.name		= "IP32 MACE ISA",
	.irq_mask	= disable_maceisa_irq,
	.irq_unmask	= enable_maceisa_irq,
};

static struct irq_chip ip32_maceisa_edge_interrupt = {
	.name		= "IP32 MACE ISA",
	.irq_ack	= mask_and_ack_maceisa_irq,
	.irq_mask	= disable_maceisa_irq,
	.irq_mask_ack	= mask_and_ack_maceisa_irq,
	.irq_unmask	= enable_maceisa_irq,
};

/* This is used for regular non-ISA, non-PCI MACE interrupts.  That means
 * bits 0-3 and 7 in the CRIME register.
 */

static void enable_mace_irq(struct irq_data *d)
{
	unsigned int bit = d->irq - CRIME_IRQ_BASE;

	crime_mask |= (1 << bit);
	crime->imask = crime_mask;
}

static void disable_mace_irq(struct irq_data *d)
{
	unsigned int bit = d->irq - CRIME_IRQ_BASE;

	crime_mask &= ~(1 << bit);
	crime->imask = crime_mask;
	flush_crime_bus();
}

static struct irq_chip ip32_mace_interrupt = {
	.name = "IP32 MACE",
	.irq_mask = disable_mace_irq,
	.irq_unmask = enable_mace_irq,
};

static void ip32_unknown_interrupt(void)
{
	printk("Unknown interrupt occurred!\n");
	printk("cp0_status: %08x\n", read_c0_status());
	printk("cp0_cause: %08x\n", read_c0_cause());
	printk("CRIME intr mask: %016lx\n", crime->imask);
	printk("CRIME intr status: %016lx\n", crime->istat);
	printk("CRIME hardware intr register: %016lx\n", crime->hard_int);
	printk("MACE ISA intr mask: %08lx\n", mace->perif.ctrl.imask);
	printk("MACE ISA intr status: %08lx\n", mace->perif.ctrl.istat);
	printk("MACE PCI control register: %08x\n", mace->pci.control);

	printk("Register dump:\n");
	show_regs(get_irq_regs());

	printk("Please mail this report to linux-mips@vger.kernel.org\n");
	printk("Spinning...");
	while(1) ;
}

/* CRIME 1.1 appears to deliver all interrupts to this one pin. */
/* change this to loop over all edge-triggered irqs, exception masked out ones */
static void ip32_irq0(void)
{
	uint64_t crime_int;
	int irq = 0;

	/*
	 * Sanity check interrupt numbering enum.
	 * MACE got 32 interrupts and there are 32 MACE ISA interrupts daisy
	 * chained.
	 */
	BUILD_BUG_ON(CRIME_VICE_IRQ - MACE_VID_IN1_IRQ != 31);
	BUILD_BUG_ON(MACEISA_SERIAL2_RDMAOR_IRQ - MACEISA_AUDIO_SW_IRQ != 31);

	crime_int = crime->istat & crime_mask;

	/* crime sometime delivers spurious interrupts, ignore them */
	if (unlikely(crime_int == 0))
		return;

	irq = MACE_VID_IN1_IRQ + __ffs(crime_int);

	if (crime_int & CRIME_MACEISA_INT_MASK) {
		unsigned long mace_int = mace->perif.ctrl.istat;
		irq = __ffs(mace_int & maceisa_mask) + MACEISA_AUDIO_SW_IRQ;
	}

	pr_debug("*irq %u*\n", irq);
	do_IRQ(irq);
}

static void ip32_irq1(void)
{
	ip32_unknown_interrupt();
}

static void ip32_irq2(void)
{
	ip32_unknown_interrupt();
}

static void ip32_irq3(void)
{
	ip32_unknown_interrupt();
}

static void ip32_irq4(void)
{
	ip32_unknown_interrupt();
}

static void ip32_irq5(void)
{
	do_IRQ(MIPS_CPU_IRQ_BASE + 7);
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending = read_c0_status() & read_c0_cause();

	if (likely(pending & IE_IRQ0))
		ip32_irq0();
	else if (unlikely(pending & IE_IRQ1))
		ip32_irq1();
	else if (unlikely(pending & IE_IRQ2))
		ip32_irq2();
	else if (unlikely(pending & IE_IRQ3))
		ip32_irq3();
	else if (unlikely(pending & IE_IRQ4))
		ip32_irq4();
	else if (likely(pending & IE_IRQ5))
		ip32_irq5();
}

void __init arch_init_irq(void)
{
	unsigned int irq;

	/* Install our interrupt handler, then clear and disable all
	 * CRIME and MACE interrupts. */
	crime->imask = 0;
	crime->hard_int = 0;
	crime->soft_int = 0;
	mace->perif.ctrl.istat = 0;
	mace->perif.ctrl.imask = 0;

	mips_cpu_irq_init();
	for (irq = CRIME_IRQ_BASE; irq <= IP32_IRQ_MAX; irq++) {
		switch (irq) {
		case MACE_VID_IN1_IRQ ... MACE_PCI_BRIDGE_IRQ:
			irq_set_chip_and_handler_name(irq,
						      &ip32_mace_interrupt,
						      handle_level_irq,
						      "level");
			break;

		case MACEPCI_SCSI0_IRQ ...  MACEPCI_SHARED2_IRQ:
			irq_set_chip_and_handler_name(irq,
						      &ip32_macepci_interrupt,
						      handle_level_irq,
						      "level");
			break;

		case CRIME_CPUERR_IRQ:
		case CRIME_MEMERR_IRQ:
			irq_set_chip_and_handler_name(irq,
						      &crime_level_interrupt,
						      handle_level_irq,
						      "level");
			break;

		case CRIME_GBE0_IRQ ... CRIME_GBE3_IRQ:
		case CRIME_RE_EMPTY_E_IRQ ... CRIME_RE_IDLE_E_IRQ:
		case CRIME_SOFT0_IRQ ... CRIME_SOFT2_IRQ:
		case CRIME_VICE_IRQ:
			irq_set_chip_and_handler_name(irq,
						      &crime_edge_interrupt,
						      handle_edge_irq,
						      "edge");
			break;

		case MACEISA_PARALLEL_IRQ:
		case MACEISA_SERIAL1_TDMAPR_IRQ:
		case MACEISA_SERIAL2_TDMAPR_IRQ:
			irq_set_chip_and_handler_name(irq,
						      &ip32_maceisa_edge_interrupt,
						      handle_edge_irq,
						      "edge");
			break;

		default:
			irq_set_chip_and_handler_name(irq,
						      &ip32_maceisa_level_interrupt,
						      handle_level_irq,
						      "level");
			break;
		}
	}
	if (request_irq(CRIME_MEMERR_IRQ, crime_memerr_intr, 0,
			"CRIME memory error", NULL))
		pr_err("Failed to register CRIME memory error interrupt\n");
	if (request_irq(CRIME_CPUERR_IRQ, crime_cpuerr_intr, 0,
			"CRIME CPU error", NULL))
		pr_err("Failed to register CRIME CPU error interrupt\n");

#define ALLINTS (IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5)
	change_c0_status(ST0_IM, ALLINTS);
}
