// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/arch/arm/mm/context.c
 *
 *  Copyright (C) 2002-2003 Deep Blue Solutions Ltd, all rights reserved.
 *  Copyright (C) 2012 ARM Limited
 *
 *  Author: Will Deacon <will.deacon@arm.com>
 */
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/percpu.h>

#include <asm/mmu_context.h>
#include <asm/smp_plat.h>
#include <asm/thread_notify.h>
#include <asm/tlbflush.h>
#include <asm/proc-fns.h>

/*
 * On ARMv6, we have the following structure in the Context ID:
 *
 * 31                         7          0
 * +-------------------------+-----------+
 * |      process ID         |   ASID    |
 * +-------------------------+-----------+
 * |              context ID             |
 * +-------------------------------------+
 *
 * The ASID is used to tag entries in the CPU caches and TLBs.
 * The context ID is used by debuggers and trace logic, and
 * should be unique within all running processes.
 *
 * In big endian operation, the two 32 bit words are swapped if accessed
 * by non-64-bit operations.
 */
#define ASID_FIRST_VERSION	(1ULL << ASID_BITS)
#define NUM_USER_ASIDS		ASID_FIRST_VERSION

static DEFINE_RAW_SPINLOCK(cpu_asid_lock);
static atomic64_t asid_generation = ATOMIC64_INIT(ASID_FIRST_VERSION);
static DECLARE_BITMAP(asid_map, NUM_USER_ASIDS);

static DEFINE_PER_CPU(atomic64_t, active_asids);
static DEFINE_PER_CPU(u64, reserved_asids);
static cpumask_t tlb_flush_pending;

#ifdef CONFIG_ARM_ERRATA_798181
void a15_erratum_get_cpumask(int this_cpu, struct mm_struct *mm,
			     cpumask_t *mask)
{
	int cpu;
	unsigned long flags;
	u64 context_id, asid;

	raw_spin_lock_irqsave(&cpu_asid_lock, flags);
	context_id = mm->context.id.counter;
	for_each_online_cpu(cpu) {
		if (cpu == this_cpu)
			continue;
		/*
		 * We only need to send an IPI if the other CPUs are
		 * running the same ASID as the one being invalidated.
		 */
		asid = per_cpu(active_asids, cpu).counter;
		if (asid == 0)
			asid = per_cpu(reserved_asids, cpu);
		if (context_id == asid)
			cpumask_set_cpu(cpu, mask);
	}
	raw_spin_unlock_irqrestore(&cpu_asid_lock, flags);
}
#endif

#ifdef CONFIG_ARM_LPAE
/*
 * With LPAE, the ASID and page tables are updated atomicly, so there is
 * no need for a reserved set of tables (the active ASID tracking prevents
 * any issues across a rollover).
 */
#define cpu_set_reserved_ttbr0()
#else
static void cpu_set_reserved_ttbr0(void)
{
	u32 ttb;
	/*
	 * Copy TTBR1 into TTBR0.
	 * This points at swapper_pg_dir, which contains only global
	 * entries so any speculative walks are perfectly safe.
	 */
	asm volatile(
	"	mrc	p15, 0, %0, c2, c0, 1		@ read TTBR1\n"
	"	mcr	p15, 0, %0, c2, c0, 0		@ set TTBR0\n"
	: "=r" (ttb));
	isb();
}
#endif

#ifdef CONFIG_PID_IN_CONTEXTIDR
static int contextidr_notifier(struct notifier_block *unused, unsigned long cmd,
			       void *t)
{
	u32 contextidr;
	pid_t pid;
	struct thread_info *thread = t;

	if (cmd != THREAD_NOTIFY_SWITCH)
		return NOTIFY_DONE;

	pid = task_pid_nr(thread->task) << ASID_BITS;
	asm volatile(
	"	mrc	p15, 0, %0, c13, c0, 1\n"
	"	and	%0, %0, %2\n"
	"	orr	%0, %0, %1\n"
	"	mcr	p15, 0, %0, c13, c0, 1\n"
	: "=r" (contextidr), "+r" (pid)
	: "I" (~ASID_MASK));
	isb();

	return NOTIFY_OK;
}

static struct notifier_block contextidr_notifier_block = {
	.notifier_call = contextidr_notifier,
};

static int __init contextidr_notifier_init(void)
{
	return thread_register_notifier(&contextidr_notifier_block);
}
arch_initcall(contextidr_notifier_init);
#endif

static void flush_context(unsigned int cpu)
{
	int i;
	u64 asid;

	/* Update the list of reserved ASIDs and the ASID bitmap. */
	bitmap_clear(asid_map, 0, NUM_USER_ASIDS);
	for_each_possible_cpu(i) {
		asid = atomic64_xchg(&per_cpu(active_asids, i), 0);
		/*
		 * If this CPU has already been through a
		 * rollover, but hasn't run another task in
		 * the meantime, we must preserve its reserved
		 * ASID, as this is the only trace we have of
		 * the process it is still running.
		 */
		if (asid == 0)
			asid = per_cpu(reserved_asids, i);
		__set_bit(asid & ~ASID_MASK, asid_map);
		per_cpu(reserved_asids, i) = asid;
	}

	/* Queue a TLB invalidate and flush the I-cache if necessary. */
	cpumask_setall(&tlb_flush_pending);

	if (icache_is_vivt_asid_tagged())
		__flush_icache_all();
}

static bool check_update_reserved_asid(u64 asid, u64 newasid)
{
	int cpu;
	bool hit = false;

	/*
	 * Iterate over the set of reserved ASIDs looking for a match.
	 * If we find one, then we can update our mm to use newasid
	 * (i.e. the same ASID in the current generation) but we can't
	 * exit the loop early, since we need to ensure that all copies
	 * of the old ASID are updated to reflect the mm. Failure to do
	 * so could result in us missing the reserved ASID in a future
	 * generation.
	 */
	for_each_possible_cpu(cpu) {
		if (per_cpu(reserved_asids, cpu) == asid) {
			hit = true;
			per_cpu(reserved_asids, cpu) = newasid;
		}
	}

	return hit;
}

static u64 new_context(struct mm_struct *mm, unsigned int cpu)
{
	static u32 cur_idx = 1;
	u64 asid = atomic64_read(&mm->context.id);
	u64 generation = atomic64_read(&asid_generation);

	if (asid != 0) {
		u64 newasid = generation | (asid & ~ASID_MASK);

		/*
		 * If our current ASID was active during a rollover, we
		 * can continue to use it and this was just a false alarm.
		 */
		if (check_update_reserved_asid(asid, newasid))
			return newasid;

		/*
		 * We had a valid ASID in a previous life, so try to re-use
		 * it if possible.,
		 */
		asid &= ~ASID_MASK;
		if (!__test_and_set_bit(asid, asid_map))
			return newasid;
	}

	/*
	 * Allocate a free ASID. If we can't find one, take a note of the
	 * currently active ASIDs and mark the TLBs as requiring flushes.
	 * We always count from ASID #1, as we reserve ASID #0 to switch
	 * via TTBR0 and to avoid speculative page table walks from hitting
	 * in any partial walk caches, which could be populated from
	 * overlapping level-1 descriptors used to map both the module
	 * area and the userspace stack.
	 */
	asid = find_next_zero_bit(asid_map, NUM_USER_ASIDS, cur_idx);
	if (asid == NUM_USER_ASIDS) {
		generation = atomic64_add_return(ASID_FIRST_VERSION,
						 &asid_generation);
		flush_context(cpu);
		asid = find_next_zero_bit(asid_map, NUM_USER_ASIDS, 1);
	}

	__set_bit(asid, asid_map);
	cur_idx = asid;
	cpumask_clear(mm_cpumask(mm));
	return asid | generation;
}

void check_and_switch_context(struct mm_struct *mm, struct task_struct *tsk)
{
	unsigned long flags;
	unsigned int cpu = smp_processor_id();
	u64 asid;

	if (unlikely(mm->context.vmalloc_seq != init_mm.context.vmalloc_seq))
		__check_vmalloc_seq(mm);

	/*
	 * We cannot update the pgd and the ASID atomicly with classic
	 * MMU, so switch exclusively to global mappings to avoid
	 * speculative page table walking with the wrong TTBR.
	 */
	cpu_set_reserved_ttbr0();

	asid = atomic64_read(&mm->context.id);
	if (!((asid ^ atomic64_read(&asid_generation)) >> ASID_BITS)
	    && atomic64_xchg(&per_cpu(active_asids, cpu), asid))
		goto switch_mm_fastpath;

	raw_spin_lock_irqsave(&cpu_asid_lock, flags);
	/* Check that our ASID belongs to the current generation. */
	asid = atomic64_read(&mm->context.id);
	if ((asid ^ atomic64_read(&asid_generation)) >> ASID_BITS) {
		asid = new_context(mm, cpu);
		atomic64_set(&mm->context.id, asid);
	}

	if (cpumask_test_and_clear_cpu(cpu, &tlb_flush_pending)) {
		local_flush_bp_all();
		local_flush_tlb_all();
	}

	atomic64_set(&per_cpu(active_asids, cpu), asid);
	cpumask_set_cpu(cpu, mm_cpumask(mm));
	raw_spin_unlock_irqrestore(&cpu_asid_lock, flags);

switch_mm_fastpath:
	cpu_switch_mm(mm->pgd, mm);
}
