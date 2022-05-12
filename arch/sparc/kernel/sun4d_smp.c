// SPDX-License-Identifier: GPL-2.0
/* Sparc SS1000/SC2000 SMP support.
 *
 * Copyright (C) 1998 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 *
 * Based on sun4m's smp.c, which is:
 * Copyright (C) 1996 David S. Miller (davem@caip.rutgers.edu)
 */

#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/profile.h>
#include <linux/delay.h>
#include <linux/sched/mm.h>
#include <linux/cpu.h>

#include <asm/cacheflush.h>
#include <asm/switch_to.h>
#include <asm/tlbflush.h>
#include <asm/timer.h>
#include <asm/oplib.h>
#include <asm/sbi.h>
#include <asm/mmu.h>

#include "kernel.h"
#include "irq.h"

#define IRQ_CROSS_CALL		15

static volatile int smp_processors_ready;
static int smp_highest_cpu;

static inline unsigned long sun4d_swap(volatile unsigned long *ptr, unsigned long val)
{
	__asm__ __volatile__("swap [%1], %0\n\t" :
			     "=&r" (val), "=&r" (ptr) :
			     "0" (val), "1" (ptr));
	return val;
}

static void smp4d_ipi_init(void);

static unsigned char cpu_leds[32];

static inline void show_leds(int cpuid)
{
	cpuid &= 0x1e;
	__asm__ __volatile__ ("stba %0, [%1] %2" : :
			      "r" ((cpu_leds[cpuid] << 4) | cpu_leds[cpuid+1]),
			      "r" (ECSR_BASE(cpuid) | BB_LEDS),
			      "i" (ASI_M_CTL));
}

void sun4d_cpu_pre_starting(void *arg)
{
	int cpuid = hard_smp_processor_id();

	/* Show we are alive */
	cpu_leds[cpuid] = 0x6;
	show_leds(cpuid);

	/* Enable level15 interrupt, disable level14 interrupt for now */
	cc_set_imsk((cc_get_imsk() & ~0x8000) | 0x4000);
}

void sun4d_cpu_pre_online(void *arg)
{
	unsigned long flags;
	int cpuid;

	cpuid = hard_smp_processor_id();

	/* Unblock the master CPU _only_ when the scheduler state
	 * of all secondary CPUs will be up-to-date, so after
	 * the SMP initialization the master will be just allowed
	 * to call the scheduler code.
	 */
	sun4d_swap((unsigned long *)&cpu_callin_map[cpuid], 1);
	local_ops->cache_all();
	local_ops->tlb_all();

	while ((unsigned long)current_set[cpuid] < PAGE_OFFSET)
		barrier();

	while (current_set[cpuid]->cpu != cpuid)
		barrier();

	/* Fix idle thread fields. */
	__asm__ __volatile__("ld [%0], %%g6\n\t"
			     : : "r" (&current_set[cpuid])
			     : "memory" /* paranoid */);

	cpu_leds[cpuid] = 0x9;
	show_leds(cpuid);

	/* Attach to the address space of init_task. */
	mmgrab(&init_mm);
	current->active_mm = &init_mm;

	local_ops->cache_all();
	local_ops->tlb_all();

	while (!cpumask_test_cpu(cpuid, &smp_commenced_mask))
		barrier();

	spin_lock_irqsave(&sun4d_imsk_lock, flags);
	cc_set_imsk(cc_get_imsk() & ~0x4000); /* Allow PIL 14 as well */
	spin_unlock_irqrestore(&sun4d_imsk_lock, flags);
}

/*
 *	Cycle through the processors asking the PROM to start each one.
 */
void __init smp4d_boot_cpus(void)
{
	smp4d_ipi_init();
	if (boot_cpu_id)
		current_set[0] = NULL;
	local_ops->cache_all();
}

int smp4d_boot_one_cpu(int i, struct task_struct *idle)
{
	unsigned long *entry = &sun4d_cpu_startup;
	int timeout;
	int cpu_node;

	cpu_find_by_instance(i, &cpu_node, NULL);
	current_set[i] = task_thread_info(idle);
	/*
	 * Initialize the contexts table
	 * Since the call to prom_startcpu() trashes the structure,
	 * we need to re-initialize it for each cpu
	 */
	smp_penguin_ctable.which_io = 0;
	smp_penguin_ctable.phys_addr = (unsigned int) srmmu_ctx_table_phys;
	smp_penguin_ctable.reg_size = 0;

	/* whirrr, whirrr, whirrrrrrrrr... */
	printk(KERN_INFO "Starting CPU %d at %p\n", i, entry);
	local_ops->cache_all();
	prom_startcpu(cpu_node,
		      &smp_penguin_ctable, 0, (char *)entry);

	printk(KERN_INFO "prom_startcpu returned :)\n");

	/* wheee... it's going... */
	for (timeout = 0; timeout < 10000; timeout++) {
		if (cpu_callin_map[i])
			break;
		udelay(200);
	}

	if (!(cpu_callin_map[i])) {
		printk(KERN_ERR "Processor %d is stuck.\n", i);
		return -ENODEV;

	}
	local_ops->cache_all();
	return 0;
}

void __init smp4d_smp_done(void)
{
	int i, first;
	int *prev;

	/* setup cpu list for irq rotation */
	first = 0;
	prev = &first;
	for_each_online_cpu(i) {
		*prev = i;
		prev = &cpu_data(i).next;
	}
	*prev = first;
	local_ops->cache_all();

	/* Ok, they are spinning and ready to go. */
	smp_processors_ready = 1;
	sun4d_distribute_irqs();
}

/* Memory structure giving interrupt handler information about IPI generated */
struct sun4d_ipi_work {
	int single;
	int msk;
	int resched;
};

static DEFINE_PER_CPU_SHARED_ALIGNED(struct sun4d_ipi_work, sun4d_ipi_work);

/* Initialize IPIs on the SUN4D SMP machine */
static void __init smp4d_ipi_init(void)
{
	int cpu;
	struct sun4d_ipi_work *work;

	printk(KERN_INFO "smp4d: setup IPI at IRQ %d\n", SUN4D_IPI_IRQ);

	for_each_possible_cpu(cpu) {
		work = &per_cpu(sun4d_ipi_work, cpu);
		work->single = work->msk = work->resched = 0;
	}
}

void sun4d_ipi_interrupt(void)
{
	struct sun4d_ipi_work *work = this_cpu_ptr(&sun4d_ipi_work);

	if (work->single) {
		work->single = 0;
		smp_call_function_single_interrupt();
	}
	if (work->msk) {
		work->msk = 0;
		smp_call_function_interrupt();
	}
	if (work->resched) {
		work->resched = 0;
		smp_resched_interrupt();
	}
}

/* +-------+-------------+-----------+------------------------------------+
 * | bcast |  devid      |   sid     |              levels mask           |
 * +-------+-------------+-----------+------------------------------------+
 *  31      30         23 22       15 14                                 0
 */
#define IGEN_MESSAGE(bcast, devid, sid, levels) \
	(((bcast) << 31) | ((devid) << 23) | ((sid) << 15) | (levels))

static void sun4d_send_ipi(int cpu, int level)
{
	cc_set_igen(IGEN_MESSAGE(0, cpu << 3, 6 + ((level >> 1) & 7), 1 << (level - 1)));
}

static void sun4d_ipi_single(int cpu)
{
	struct sun4d_ipi_work *work = &per_cpu(sun4d_ipi_work, cpu);

	/* Mark work */
	work->single = 1;

	/* Generate IRQ on the CPU */
	sun4d_send_ipi(cpu, SUN4D_IPI_IRQ);
}

static void sun4d_ipi_mask_one(int cpu)
{
	struct sun4d_ipi_work *work = &per_cpu(sun4d_ipi_work, cpu);

	/* Mark work */
	work->msk = 1;

	/* Generate IRQ on the CPU */
	sun4d_send_ipi(cpu, SUN4D_IPI_IRQ);
}

static void sun4d_ipi_resched(int cpu)
{
	struct sun4d_ipi_work *work = &per_cpu(sun4d_ipi_work, cpu);

	/* Mark work */
	work->resched = 1;

	/* Generate IRQ on the CPU (any IRQ will cause resched) */
	sun4d_send_ipi(cpu, SUN4D_IPI_IRQ);
}

static struct smp_funcall {
	smpfunc_t func;
	unsigned long arg1;
	unsigned long arg2;
	unsigned long arg3;
	unsigned long arg4;
	unsigned long arg5;
	unsigned char processors_in[NR_CPUS];  /* Set when ipi entered. */
	unsigned char processors_out[NR_CPUS]; /* Set when ipi exited. */
} ccall_info __attribute__((aligned(8)));

static DEFINE_SPINLOCK(cross_call_lock);

/* Cross calls must be serialized, at least currently. */
static void sun4d_cross_call(smpfunc_t func, cpumask_t mask, unsigned long arg1,
			     unsigned long arg2, unsigned long arg3,
			     unsigned long arg4)
{
	if (smp_processors_ready) {
		register int high = smp_highest_cpu;
		unsigned long flags;

		spin_lock_irqsave(&cross_call_lock, flags);

		{
			/*
			 * If you make changes here, make sure
			 * gcc generates proper code...
			 */
			register smpfunc_t f asm("i0") = func;
			register unsigned long a1 asm("i1") = arg1;
			register unsigned long a2 asm("i2") = arg2;
			register unsigned long a3 asm("i3") = arg3;
			register unsigned long a4 asm("i4") = arg4;
			register unsigned long a5 asm("i5") = 0;

			__asm__ __volatile__(
				"std %0, [%6]\n\t"
				"std %2, [%6 + 8]\n\t"
				"std %4, [%6 + 16]\n\t" : :
				"r"(f), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
				"r" (&ccall_info.func));
		}

		/* Init receive/complete mapping, plus fire the IPI's off. */
		{
			register int i;

			cpumask_clear_cpu(smp_processor_id(), &mask);
			cpumask_and(&mask, cpu_online_mask, &mask);
			for (i = 0; i <= high; i++) {
				if (cpumask_test_cpu(i, &mask)) {
					ccall_info.processors_in[i] = 0;
					ccall_info.processors_out[i] = 0;
					sun4d_send_ipi(i, IRQ_CROSS_CALL);
				}
			}
		}

		{
			register int i;

			i = 0;
			do {
				if (!cpumask_test_cpu(i, &mask))
					continue;
				while (!ccall_info.processors_in[i])
					barrier();
			} while (++i <= high);

			i = 0;
			do {
				if (!cpumask_test_cpu(i, &mask))
					continue;
				while (!ccall_info.processors_out[i])
					barrier();
			} while (++i <= high);
		}

		spin_unlock_irqrestore(&cross_call_lock, flags);
	}
}

/* Running cross calls. */
void smp4d_cross_call_irq(void)
{
	int i = hard_smp_processor_id();

	ccall_info.processors_in[i] = 1;
	ccall_info.func(ccall_info.arg1, ccall_info.arg2, ccall_info.arg3,
			ccall_info.arg4, ccall_info.arg5);
	ccall_info.processors_out[i] = 1;
}

void smp4d_percpu_timer_interrupt(struct pt_regs *regs)
{
	struct pt_regs *old_regs;
	int cpu = hard_smp_processor_id();
	struct clock_event_device *ce;
	static int cpu_tick[NR_CPUS];
	static char led_mask[] = { 0xe, 0xd, 0xb, 0x7, 0xb, 0xd };

	old_regs = set_irq_regs(regs);
	bw_get_prof_limit(cpu);
	bw_clear_intr_mask(0, 1);	/* INTR_TABLE[0] & 1 is Profile IRQ */

	cpu_tick[cpu]++;
	if (!(cpu_tick[cpu] & 15)) {
		if (cpu_tick[cpu] == 0x60)
			cpu_tick[cpu] = 0;
		cpu_leds[cpu] = led_mask[cpu_tick[cpu] >> 4];
		show_leds(cpu);
	}

	ce = &per_cpu(sparc32_clockevent, cpu);

	irq_enter();
	ce->event_handler(ce);
	irq_exit();

	set_irq_regs(old_regs);
}

static const struct sparc32_ipi_ops sun4d_ipi_ops = {
	.cross_call = sun4d_cross_call,
	.resched    = sun4d_ipi_resched,
	.single     = sun4d_ipi_single,
	.mask_one   = sun4d_ipi_mask_one,
};

void __init sun4d_init_smp(void)
{
	int i;

	/* Patch ipi15 trap table */
	t_nmi[1] = t_nmi[1] + (linux_trap_ipi15_sun4d - linux_trap_ipi15_sun4m);

	sparc32_ipi_ops = &sun4d_ipi_ops;

	for (i = 0; i < NR_CPUS; i++) {
		ccall_info.processors_in[i] = 1;
		ccall_info.processors_out[i] = 1;
	}
}
