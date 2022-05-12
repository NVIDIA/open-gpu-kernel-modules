/*
 * Copyright 2003-2011 NetLogic Microsystems, Inc. (NetLogic). All rights
 * reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the NetLogic
 * license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NETLOGIC ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL NETLOGIC OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/sched/task_stack.h>
#include <linux/smp.h>
#include <linux/irq.h>

#include <asm/mmu_context.h>

#include <asm/netlogic/interrupt.h>
#include <asm/netlogic/mips-extns.h>
#include <asm/netlogic/haldefs.h>
#include <asm/netlogic/common.h>

#if defined(CONFIG_CPU_XLP)
#include <asm/netlogic/xlp-hal/iomap.h>
#include <asm/netlogic/xlp-hal/xlp.h>
#include <asm/netlogic/xlp-hal/pic.h>
#elif defined(CONFIG_CPU_XLR)
#include <asm/netlogic/xlr/iomap.h>
#include <asm/netlogic/xlr/pic.h>
#include <asm/netlogic/xlr/xlr.h>
#else
#error "Unknown CPU"
#endif

void nlm_send_ipi_single(int logical_cpu, unsigned int action)
{
	unsigned int hwtid;
	uint64_t picbase;

	/* node id is part of hwtid, and needed for send_ipi */
	hwtid = cpu_logical_map(logical_cpu);
	picbase = nlm_get_node(nlm_hwtid_to_node(hwtid))->picbase;

	if (action & SMP_CALL_FUNCTION)
		nlm_pic_send_ipi(picbase, hwtid, IRQ_IPI_SMP_FUNCTION, 0);
	if (action & SMP_RESCHEDULE_YOURSELF)
		nlm_pic_send_ipi(picbase, hwtid, IRQ_IPI_SMP_RESCHEDULE, 0);
}

void nlm_send_ipi_mask(const struct cpumask *mask, unsigned int action)
{
	int cpu;

	for_each_cpu(cpu, mask) {
		nlm_send_ipi_single(cpu, action);
	}
}

/* IRQ_IPI_SMP_FUNCTION Handler */
void nlm_smp_function_ipi_handler(struct irq_desc *desc)
{
	unsigned int irq = irq_desc_get_irq(desc);
	clear_c0_eimr(irq);
	ack_c0_eirr(irq);
	generic_smp_call_function_interrupt();
	set_c0_eimr(irq);
}

/* IRQ_IPI_SMP_RESCHEDULE  handler */
void nlm_smp_resched_ipi_handler(struct irq_desc *desc)
{
	unsigned int irq = irq_desc_get_irq(desc);
	clear_c0_eimr(irq);
	ack_c0_eirr(irq);
	scheduler_ipi();
	set_c0_eimr(irq);
}

/*
 * Called before going into mips code, early cpu init
 */
void nlm_early_init_secondary(int cpu)
{
	change_c0_config(CONF_CM_CMASK, 0x3);
#ifdef CONFIG_CPU_XLP
	xlp_mmu_init();
#endif
	write_c0_ebase(nlm_current_node()->ebase);
}

/*
 * Code to run on secondary just after probing the CPU
 */
static void nlm_init_secondary(void)
{
	int hwtid;

	hwtid = hard_smp_processor_id();
	cpu_set_core(&current_cpu_data, hwtid / NLM_THREADS_PER_CORE);
	current_cpu_data.package = nlm_nodeid();
	nlm_percpu_init(hwtid);
	nlm_smp_irq_init(hwtid);
}

void nlm_prepare_cpus(unsigned int max_cpus)
{
	/* declare we are SMT capable */
	smp_num_siblings = nlm_threads_per_core;
}

void nlm_smp_finish(void)
{
	local_irq_enable();
}

/*
 * Boot all other cpus in the system, initialize them, and bring them into
 * the boot function
 */
unsigned long nlm_next_gp;
unsigned long nlm_next_sp;
static cpumask_t phys_cpu_present_mask;

int nlm_boot_secondary(int logical_cpu, struct task_struct *idle)
{
	uint64_t picbase;
	int hwtid;

	hwtid = cpu_logical_map(logical_cpu);
	picbase = nlm_get_node(nlm_hwtid_to_node(hwtid))->picbase;

	nlm_next_sp = (unsigned long)__KSTK_TOS(idle);
	nlm_next_gp = (unsigned long)task_thread_info(idle);

	/* barrier for sp/gp store above */
	__sync();
	nlm_pic_send_ipi(picbase, hwtid, 1, 1);  /* NMI */

	return 0;
}

void __init nlm_smp_setup(void)
{
	unsigned int boot_cpu;
	int num_cpus, i, ncore, node;
	volatile u32 *cpu_ready = nlm_get_boot_data(BOOT_CPU_READY);

	boot_cpu = hard_smp_processor_id();
	cpumask_clear(&phys_cpu_present_mask);

	cpumask_set_cpu(boot_cpu, &phys_cpu_present_mask);
	__cpu_number_map[boot_cpu] = 0;
	__cpu_logical_map[0] = boot_cpu;
	set_cpu_possible(0, true);

	num_cpus = 1;
	for (i = 0; i < NR_CPUS; i++) {
		/*
		 * cpu_ready array is not set for the boot_cpu,
		 * it is only set for ASPs (see smpboot.S)
		 */
		if (cpu_ready[i]) {
			cpumask_set_cpu(i, &phys_cpu_present_mask);
			__cpu_number_map[i] = num_cpus;
			__cpu_logical_map[num_cpus] = i;
			set_cpu_possible(num_cpus, true);
			node = nlm_hwtid_to_node(i);
			cpumask_set_cpu(num_cpus, &nlm_get_node(node)->cpumask);
			++num_cpus;
		}
	}

	pr_info("Physical CPU mask: %*pb\n",
		cpumask_pr_args(&phys_cpu_present_mask));
	pr_info("Possible CPU mask: %*pb\n",
		cpumask_pr_args(cpu_possible_mask));

	/* check with the cores we have woken up */
	for (ncore = 0, i = 0; i < NLM_NR_NODES; i++)
		ncore += hweight32(nlm_get_node(i)->coremask);

	pr_info("Detected (%dc%dt) %d Slave CPU(s)\n", ncore,
		nlm_threads_per_core, num_cpus);

	/* switch NMI handler to boot CPUs */
	nlm_set_nmi_handler(nlm_boot_secondary_cpus);
}

static int nlm_parse_cpumask(cpumask_t *wakeup_mask)
{
	uint32_t core0_thr_mask, core_thr_mask;
	int threadmode, i, j;

	core0_thr_mask = 0;
	for (i = 0; i < NLM_THREADS_PER_CORE; i++)
		if (cpumask_test_cpu(i, wakeup_mask))
			core0_thr_mask |= (1 << i);
	switch (core0_thr_mask) {
	case 1:
		nlm_threads_per_core = 1;
		threadmode = 0;
		break;
	case 3:
		nlm_threads_per_core = 2;
		threadmode = 2;
		break;
	case 0xf:
		nlm_threads_per_core = 4;
		threadmode = 3;
		break;
	default:
		goto unsupp;
	}

	/* Verify other cores CPU masks */
	for (i = 0; i < NR_CPUS; i += NLM_THREADS_PER_CORE) {
		core_thr_mask = 0;
		for (j = 0; j < NLM_THREADS_PER_CORE; j++)
			if (cpumask_test_cpu(i + j, wakeup_mask))
				core_thr_mask |= (1 << j);
		if (core_thr_mask != 0 && core_thr_mask != core0_thr_mask)
				goto unsupp;
	}
	return threadmode;

unsupp:
	panic("Unsupported CPU mask %*pb", cpumask_pr_args(wakeup_mask));
	return 0;
}

int nlm_wakeup_secondary_cpus(void)
{
	u32 *reset_data;
	int threadmode;

	/* verify the mask and setup core config variables */
	threadmode = nlm_parse_cpumask(&nlm_cpumask);

	/* Setup CPU init parameters */
	reset_data = nlm_get_boot_data(BOOT_THREAD_MODE);
	*reset_data = threadmode;

#ifdef CONFIG_CPU_XLP
	xlp_wakeup_secondary_cpus();
#else
	xlr_wakeup_secondary_cpus();
#endif
	return 0;
}

const struct plat_smp_ops nlm_smp_ops = {
	.send_ipi_single	= nlm_send_ipi_single,
	.send_ipi_mask		= nlm_send_ipi_mask,
	.init_secondary		= nlm_init_secondary,
	.smp_finish		= nlm_smp_finish,
	.boot_secondary		= nlm_boot_secondary,
	.smp_setup		= nlm_smp_setup,
	.prepare_cpus		= nlm_prepare_cpus,
};
