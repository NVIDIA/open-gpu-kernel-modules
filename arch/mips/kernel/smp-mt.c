// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Copyright (C) 2004, 05, 06 MIPS Technologies, Inc.
 *    Elizabeth Clarke (beth@mips.com)
 *    Ralf Baechle (ralf@linux-mips.org)
 * Copyright (C) 2006 Ralf Baechle (ralf@linux-mips.org)
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/interrupt.h>
#include <linux/compiler.h>
#include <linux/sched/task_stack.h>
#include <linux/smp.h>

#include <linux/atomic.h>
#include <asm/cacheflush.h>
#include <asm/cpu.h>
#include <asm/processor.h>
#include <asm/hardirq.h>
#include <asm/mmu_context.h>
#include <asm/time.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/mips_mt.h>
#include <asm/mips-cps.h>

static void __init smvp_copy_vpe_config(void)
{
	write_vpe_c0_status(
		(read_c0_status() & ~(ST0_IM | ST0_IE | ST0_KSU)) | ST0_CU0);

	/* set config to be the same as vpe0, particularly kseg0 coherency alg */
	write_vpe_c0_config( read_c0_config());

	/* make sure there are no software interrupts pending */
	write_vpe_c0_cause(0);

	/* Propagate Config7 */
	write_vpe_c0_config7(read_c0_config7());

	write_vpe_c0_count(read_c0_count());
}

static unsigned int __init smvp_vpe_init(unsigned int tc, unsigned int mvpconf0,
	unsigned int ncpu)
{
	if (tc > ((mvpconf0 & MVPCONF0_PVPE) >> MVPCONF0_PVPE_SHIFT))
		return ncpu;

	/* Deactivate all but VPE 0 */
	if (tc != 0) {
		unsigned long tmp = read_vpe_c0_vpeconf0();

		tmp &= ~VPECONF0_VPA;

		/* master VPE */
		tmp |= VPECONF0_MVP;
		write_vpe_c0_vpeconf0(tmp);

		/* Record this as available CPU */
		set_cpu_possible(tc, true);
		set_cpu_present(tc, true);
		__cpu_number_map[tc]	= ++ncpu;
		__cpu_logical_map[ncpu] = tc;
	}

	/* Disable multi-threading with TC's */
	write_vpe_c0_vpecontrol(read_vpe_c0_vpecontrol() & ~VPECONTROL_TE);

	if (tc != 0)
		smvp_copy_vpe_config();

	cpu_set_vpe_id(&cpu_data[ncpu], tc);

	return ncpu;
}

static void __init smvp_tc_init(unsigned int tc, unsigned int mvpconf0)
{
	unsigned long tmp;

	if (!tc)
		return;

	/* bind a TC to each VPE, May as well put all excess TC's
	   on the last VPE */
	if (tc >= (((mvpconf0 & MVPCONF0_PVPE) >> MVPCONF0_PVPE_SHIFT)+1))
		write_tc_c0_tcbind(read_tc_c0_tcbind() | ((mvpconf0 & MVPCONF0_PVPE) >> MVPCONF0_PVPE_SHIFT));
	else {
		write_tc_c0_tcbind(read_tc_c0_tcbind() | tc);

		/* and set XTC */
		write_vpe_c0_vpeconf0(read_vpe_c0_vpeconf0() | (tc << VPECONF0_XTC_SHIFT));
	}

	tmp = read_tc_c0_tcstatus();

	/* mark not allocated and not dynamically allocatable */
	tmp &= ~(TCSTATUS_A | TCSTATUS_DA);
	tmp |= TCSTATUS_IXMT;		/* interrupt exempt */
	write_tc_c0_tcstatus(tmp);

	write_tc_c0_tchalt(TCHALT_H);
}

static void vsmp_init_secondary(void)
{
	/* This is Malta specific: IPI,performance and timer interrupts */
	if (mips_gic_present())
		change_c0_status(ST0_IM, STATUSF_IP2 | STATUSF_IP3 |
					 STATUSF_IP4 | STATUSF_IP5 |
					 STATUSF_IP6 | STATUSF_IP7);
	else
		change_c0_status(ST0_IM, STATUSF_IP0 | STATUSF_IP1 |
					 STATUSF_IP6 | STATUSF_IP7);
}

static void vsmp_smp_finish(void)
{
	/* CDFIXME: remove this? */
	write_c0_compare(read_c0_count() + (8* mips_hpt_frequency/HZ));

#ifdef CONFIG_MIPS_MT_FPAFF
	/* If we have an FPU, enroll ourselves in the FPU-full mask */
	if (cpu_has_fpu)
		cpumask_set_cpu(smp_processor_id(), &mt_fpu_cpumask);
#endif /* CONFIG_MIPS_MT_FPAFF */

	local_irq_enable();
}

/*
 * Setup the PC, SP, and GP of a secondary processor and start it
 * running!
 * smp_bootstrap is the place to resume from
 * __KSTK_TOS(idle) is apparently the stack pointer
 * (unsigned long)idle->thread_info the gp
 * assumes a 1:1 mapping of TC => VPE
 */
static int vsmp_boot_secondary(int cpu, struct task_struct *idle)
{
	struct thread_info *gp = task_thread_info(idle);
	dvpe();
	set_c0_mvpcontrol(MVPCONTROL_VPC);

	settc(cpu);

	/* restart */
	write_tc_c0_tcrestart((unsigned long)&smp_bootstrap);

	/* enable the tc this vpe/cpu will be running */
	write_tc_c0_tcstatus((read_tc_c0_tcstatus() & ~TCSTATUS_IXMT) | TCSTATUS_A);

	write_tc_c0_tchalt(0);

	/* enable the VPE */
	write_vpe_c0_vpeconf0(read_vpe_c0_vpeconf0() | VPECONF0_VPA);

	/* stack pointer */
	write_tc_gpr_sp( __KSTK_TOS(idle));

	/* global pointer */
	write_tc_gpr_gp((unsigned long)gp);

	flush_icache_range((unsigned long)gp,
			   (unsigned long)(gp + sizeof(struct thread_info)));

	/* finally out of configuration and into chaos */
	clear_c0_mvpcontrol(MVPCONTROL_VPC);

	evpe(EVPE_ENABLE);

	return 0;
}

/*
 * Common setup before any secondaries are started
 * Make sure all CPU's are in a sensible state before we boot any of the
 * secondaries
 */
static void __init vsmp_smp_setup(void)
{
	unsigned int mvpconf0, ntc, tc, ncpu = 0;
	unsigned int nvpe;

#ifdef CONFIG_MIPS_MT_FPAFF
	/* If we have an FPU, enroll ourselves in the FPU-full mask */
	if (cpu_has_fpu)
		cpumask_set_cpu(0, &mt_fpu_cpumask);
#endif /* CONFIG_MIPS_MT_FPAFF */
	if (!cpu_has_mipsmt)
		return;

	/* disable MT so we can configure */
	dvpe();
	dmt();

	/* Put MVPE's into 'configuration state' */
	set_c0_mvpcontrol(MVPCONTROL_VPC);

	mvpconf0 = read_c0_mvpconf0();
	ntc = (mvpconf0 & MVPCONF0_PTC) >> MVPCONF0_PTC_SHIFT;

	nvpe = ((mvpconf0 & MVPCONF0_PVPE) >> MVPCONF0_PVPE_SHIFT) + 1;
	smp_num_siblings = nvpe;

	/* we'll always have more TC's than VPE's, so loop setting everything
	   to a sensible state */
	for (tc = 0; tc <= ntc; tc++) {
		settc(tc);

		smvp_tc_init(tc, mvpconf0);
		ncpu = smvp_vpe_init(tc, mvpconf0, ncpu);
	}

	/* Release config state */
	clear_c0_mvpcontrol(MVPCONTROL_VPC);

	/* We'll wait until starting the secondaries before starting MVPE */

	printk(KERN_INFO "Detected %i available secondary CPU(s)\n", ncpu);
}

static void __init vsmp_prepare_cpus(unsigned int max_cpus)
{
	mips_mt_set_cpuoptions();
}

const struct plat_smp_ops vsmp_smp_ops = {
	.send_ipi_single	= mips_smp_send_ipi_single,
	.send_ipi_mask		= mips_smp_send_ipi_mask,
	.init_secondary		= vsmp_init_secondary,
	.smp_finish		= vsmp_smp_finish,
	.boot_secondary		= vsmp_boot_secondary,
	.smp_setup		= vsmp_smp_setup,
	.prepare_cpus		= vsmp_prepare_cpus,
};

