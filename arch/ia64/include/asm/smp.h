/* SPDX-License-Identifier: GPL-2.0 */
/*
 * SMP Support
 *
 * Copyright (C) 1999 VA Linux Systems
 * Copyright (C) 1999 Walt Drummond <drummond@valinux.com>
 * (c) Copyright 2001-2003, 2005 Hewlett-Packard Development Company, L.P.
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 *	Bjorn Helgaas <bjorn.helgaas@hp.com>
 */
#ifndef _ASM_IA64_SMP_H
#define _ASM_IA64_SMP_H

#include <linux/init.h>
#include <linux/threads.h>
#include <linux/kernel.h>
#include <linux/cpumask.h>
#include <linux/bitops.h>
#include <linux/irqreturn.h>

#include <asm/param.h>
#include <asm/processor.h>
#include <asm/ptrace.h>

static inline unsigned int
ia64_get_lid (void)
{
	union {
		struct {
			unsigned long reserved : 16;
			unsigned long eid : 8;
			unsigned long id : 8;
			unsigned long ignored : 32;
		} f;
		unsigned long bits;
	} lid;

	lid.bits = ia64_getreg(_IA64_REG_CR_LID);
	return lid.f.id << 8 | lid.f.eid;
}

#define hard_smp_processor_id()		ia64_get_lid()

#ifdef CONFIG_SMP

#define raw_smp_processor_id() (current_thread_info()->cpu)

extern struct smp_boot_data {
	int cpu_count;
	int cpu_phys_id[NR_CPUS];
} smp_boot_data __initdata;

extern char no_int_routing;

extern cpumask_t cpu_core_map[NR_CPUS];
DECLARE_PER_CPU_SHARED_ALIGNED(cpumask_t, cpu_sibling_map);
extern int smp_num_siblings;
extern void __iomem *ipi_base_addr;

extern volatile int ia64_cpu_to_sapicid[];
#define cpu_physical_id(i)	ia64_cpu_to_sapicid[i]

extern unsigned long ap_wakeup_vector;

/*
 * Function to map hard smp processor id to logical id.  Slow, so don't use this in
 * performance-critical code.
 */
static inline int
cpu_logical_id (int cpuid)
{
	int i;

	for (i = 0; i < NR_CPUS; ++i)
		if (cpu_physical_id(i) == cpuid)
			break;
	return i;
}

/* Upping and downing of CPUs */
extern int __cpu_disable (void);
extern void __cpu_die (unsigned int cpu);
extern void cpu_die (void) __attribute__ ((noreturn));
extern void __init smp_build_cpu_map(void);

extern void __init init_smp_config (void);
extern void smp_do_timer (struct pt_regs *regs);

extern irqreturn_t handle_IPI(int irq, void *dev_id);
extern void smp_send_reschedule (int cpu);
extern void identify_siblings (struct cpuinfo_ia64 *);
extern int is_multithreading_enabled(void);

extern void arch_send_call_function_single_ipi(int cpu);
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);

#else /* CONFIG_SMP */

#define cpu_logical_id(i)		0
#define cpu_physical_id(i)		ia64_get_lid()

#endif /* CONFIG_SMP */
#endif /* _ASM_IA64_SMP_H */
