/*
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of this
 * archive for more details.
 *
 * Copyright (C) 2000 - 2001 by Kanoj Sarcar (kanoj@sgi.com)
 * Copyright (C) 2000 - 2001 by Silicon Graphics, Inc.
 */
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/task_stack.h>
#include <linux/topology.h>
#include <linux/nodemask.h>

#include <asm/page.h>
#include <asm/processor.h>
#include <asm/ptrace.h>
#include <asm/sn/agent.h>
#include <asm/sn/arch.h>
#include <asm/sn/gda.h>
#include <asm/sn/intr.h>
#include <asm/sn/klconfig.h>
#include <asm/sn/launch.h>
#include <asm/sn/mapped_kernel.h>
#include <asm/sn/types.h>

#include "ip27-common.h"

static int node_scan_cpus(nasid_t nasid, int highest)
{
	static int cpus_found;
	lboard_t *brd;
	klcpu_t *acpu;
	cpuid_t cpuid;

	brd = find_lboard((lboard_t *)KL_CONFIG_INFO(nasid), KLTYPE_IP27);

	do {
		acpu = (klcpu_t *)find_first_component(brd, KLSTRUCT_CPU);
		while (acpu) {
			cpuid = acpu->cpu_info.virtid;
			/* Only let it join in if it's marked enabled */
			if ((acpu->cpu_info.flags & KLINFO_ENABLE) &&
			    (cpus_found != NR_CPUS)) {
				if (cpuid > highest)
					highest = cpuid;
				set_cpu_possible(cpuid, true);
				cputonasid(cpus_found) = nasid;
				cputoslice(cpus_found) = acpu->cpu_info.physid;
				sn_cpu_info[cpus_found].p_speed =
							acpu->cpu_speed;
				cpus_found++;
			}
			acpu = (klcpu_t *)find_component(brd, (klinfo_t *)acpu,
								KLSTRUCT_CPU);
		}
		brd = KLCF_NEXT(brd);
		if (!brd)
			break;

		brd = find_lboard(brd, KLTYPE_IP27);
	} while (brd);

	return highest;
}

void cpu_node_probe(void)
{
	int i, highest = 0;
	gda_t *gdap = GDA;

	nodes_clear(node_online_map);
	for (i = 0; i < MAX_NUMNODES; i++) {
		nasid_t nasid = gdap->g_nasidtable[i];
		if (nasid == INVALID_NASID)
			break;
		node_set_online(nasid);
		highest = node_scan_cpus(nasid, highest);
	}

	printk("Discovered %d cpus on %d nodes\n", highest + 1, num_online_nodes());
}

static __init void intr_clear_all(nasid_t nasid)
{
	int i;

	REMOTE_HUB_S(nasid, PI_INT_MASK0_A, 0);
	REMOTE_HUB_S(nasid, PI_INT_MASK0_B, 0);
	REMOTE_HUB_S(nasid, PI_INT_MASK1_A, 0);
	REMOTE_HUB_S(nasid, PI_INT_MASK1_B, 0);

	for (i = 0; i < 128; i++)
		REMOTE_HUB_CLR_INTR(nasid, i);
}

static void ip27_send_ipi_single(int destid, unsigned int action)
{
	int irq;

	switch (action) {
	case SMP_RESCHEDULE_YOURSELF:
		irq = CPU_RESCHED_A_IRQ;
		break;
	case SMP_CALL_FUNCTION:
		irq = CPU_CALL_A_IRQ;
		break;
	default:
		panic("sendintr");
	}

	irq += cputoslice(destid);

	/*
	 * Set the interrupt bit associated with the CPU we want to
	 * send the interrupt to.
	 */
	REMOTE_HUB_SEND_INTR(cpu_to_node(destid), irq);
}

static void ip27_send_ipi_mask(const struct cpumask *mask, unsigned int action)
{
	unsigned int i;

	for_each_cpu(i, mask)
		ip27_send_ipi_single(i, action);
}

static void ip27_init_cpu(void)
{
	per_cpu_init();
}

static void ip27_smp_finish(void)
{
	hub_rt_clock_event_init();
	local_irq_enable();
}

/*
 * Launch a slave into smp_bootstrap().	 It doesn't take an argument, and we
 * set sp to the kernel stack of the newly created idle process, gp to the proc
 * struct so that current_thread_info() will work.
 */
static int ip27_boot_secondary(int cpu, struct task_struct *idle)
{
	unsigned long gp = (unsigned long)task_thread_info(idle);
	unsigned long sp = __KSTK_TOS(idle);

	LAUNCH_SLAVE(cputonasid(cpu), cputoslice(cpu),
		(launch_proc_t)MAPPED_KERN_RW_TO_K0(smp_bootstrap),
		0, (void *) sp, (void *) gp);
	return 0;
}

static void __init ip27_smp_setup(void)
{
	nasid_t nasid;

	for_each_online_node(nasid) {
		if (nasid == 0)
			continue;
		intr_clear_all(nasid);
	}

	replicate_kernel_text();

	/*
	 * PROM sets up system, that boot cpu is always first CPU on nasid 0
	 */
	cputonasid(0) = 0;
	cputoslice(0) = LOCAL_HUB_L(PI_CPU_NUM);
}

static void __init ip27_prepare_cpus(unsigned int max_cpus)
{
	/* We already did everything necessary earlier */
}

const struct plat_smp_ops ip27_smp_ops = {
	.send_ipi_single	= ip27_send_ipi_single,
	.send_ipi_mask		= ip27_send_ipi_mask,
	.init_secondary		= ip27_init_cpu,
	.smp_finish		= ip27_smp_finish,
	.boot_secondary		= ip27_boot_secondary,
	.smp_setup		= ip27_smp_setup,
	.prepare_cpus		= ip27_prepare_cpus,
	.prepare_boot_cpu	= ip27_init_cpu,
};
