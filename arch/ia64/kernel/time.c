// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/ia64/kernel/time.c
 *
 * Copyright (C) 1998-2003 Hewlett-Packard Co
 *	Stephane Eranian <eranian@hpl.hp.com>
 *	David Mosberger <davidm@hpl.hp.com>
 * Copyright (C) 1999 Don Dugger <don.dugger@intel.com>
 * Copyright (C) 1999-2000 VA Linux Systems
 * Copyright (C) 1999-2000 Walt Drummond <drummond@valinux.com>
 */

#include <linux/cpu.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/profile.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/nmi.h>
#include <linux/interrupt.h>
#include <linux/efi.h>
#include <linux/timex.h>
#include <linux/timekeeper_internal.h>
#include <linux/platform_device.h>
#include <linux/sched/cputime.h>

#include <asm/delay.h>
#include <asm/efi.h>
#include <asm/hw_irq.h>
#include <asm/ptrace.h>
#include <asm/sal.h>
#include <asm/sections.h>

#include "fsyscall_gtod_data.h"
#include "irq.h"

static u64 itc_get_cycles(struct clocksource *cs);

struct fsyscall_gtod_data_t fsyscall_gtod_data;

struct itc_jitter_data_t itc_jitter_data;

volatile int time_keeper_id = 0; /* smp_processor_id() of time-keeper */

#ifdef CONFIG_IA64_DEBUG_IRQ

unsigned long last_cli_ip;
EXPORT_SYMBOL(last_cli_ip);

#endif

static struct clocksource clocksource_itc = {
	.name           = "itc",
	.rating         = 350,
	.read           = itc_get_cycles,
	.mask           = CLOCKSOURCE_MASK(64),
	.flags          = CLOCK_SOURCE_IS_CONTINUOUS,
};
static struct clocksource *itc_clocksource;

#ifdef CONFIG_VIRT_CPU_ACCOUNTING_NATIVE

#include <linux/kernel_stat.h>

extern u64 cycle_to_nsec(u64 cyc);

void vtime_flush(struct task_struct *tsk)
{
	struct thread_info *ti = task_thread_info(tsk);
	u64 delta;

	if (ti->utime)
		account_user_time(tsk, cycle_to_nsec(ti->utime));

	if (ti->gtime)
		account_guest_time(tsk, cycle_to_nsec(ti->gtime));

	if (ti->idle_time)
		account_idle_time(cycle_to_nsec(ti->idle_time));

	if (ti->stime) {
		delta = cycle_to_nsec(ti->stime);
		account_system_index_time(tsk, delta, CPUTIME_SYSTEM);
	}

	if (ti->hardirq_time) {
		delta = cycle_to_nsec(ti->hardirq_time);
		account_system_index_time(tsk, delta, CPUTIME_IRQ);
	}

	if (ti->softirq_time) {
		delta = cycle_to_nsec(ti->softirq_time);
		account_system_index_time(tsk, delta, CPUTIME_SOFTIRQ);
	}

	ti->utime = 0;
	ti->gtime = 0;
	ti->idle_time = 0;
	ti->stime = 0;
	ti->hardirq_time = 0;
	ti->softirq_time = 0;
}

/*
 * Called from the context switch with interrupts disabled, to charge all
 * accumulated times to the current process, and to prepare accounting on
 * the next process.
 */
void arch_vtime_task_switch(struct task_struct *prev)
{
	struct thread_info *pi = task_thread_info(prev);
	struct thread_info *ni = task_thread_info(current);

	ni->ac_stamp = pi->ac_stamp;
	ni->ac_stime = ni->ac_utime = 0;
}

/*
 * Account time for a transition between system, hard irq or soft irq state.
 * Note that this function is called with interrupts enabled.
 */
static __u64 vtime_delta(struct task_struct *tsk)
{
	struct thread_info *ti = task_thread_info(tsk);
	__u64 now, delta_stime;

	WARN_ON_ONCE(!irqs_disabled());

	now = ia64_get_itc();
	delta_stime = now - ti->ac_stamp;
	ti->ac_stamp = now;

	return delta_stime;
}

void vtime_account_kernel(struct task_struct *tsk)
{
	struct thread_info *ti = task_thread_info(tsk);
	__u64 stime = vtime_delta(tsk);

	if (tsk->flags & PF_VCPU)
		ti->gtime += stime;
	else
		ti->stime += stime;
}
EXPORT_SYMBOL_GPL(vtime_account_kernel);

void vtime_account_idle(struct task_struct *tsk)
{
	struct thread_info *ti = task_thread_info(tsk);

	ti->idle_time += vtime_delta(tsk);
}

void vtime_account_softirq(struct task_struct *tsk)
{
	struct thread_info *ti = task_thread_info(tsk);

	ti->softirq_time += vtime_delta(tsk);
}

void vtime_account_hardirq(struct task_struct *tsk)
{
	struct thread_info *ti = task_thread_info(tsk);

	ti->hardirq_time += vtime_delta(tsk);
}

#endif /* CONFIG_VIRT_CPU_ACCOUNTING_NATIVE */

static irqreturn_t
timer_interrupt (int irq, void *dev_id)
{
	unsigned long new_itm;

	if (cpu_is_offline(smp_processor_id())) {
		return IRQ_HANDLED;
	}

	new_itm = local_cpu_data->itm_next;

	if (!time_after(ia64_get_itc(), new_itm))
		printk(KERN_ERR "Oops: timer tick before it's due (itc=%lx,itm=%lx)\n",
		       ia64_get_itc(), new_itm);

	while (1) {
		new_itm += local_cpu_data->itm_delta;

		legacy_timer_tick(smp_processor_id() == time_keeper_id);

		local_cpu_data->itm_next = new_itm;

		if (time_after(new_itm, ia64_get_itc()))
			break;

		/*
		 * Allow IPIs to interrupt the timer loop.
		 */
		local_irq_enable();
		local_irq_disable();
	}

	do {
		/*
		 * If we're too close to the next clock tick for
		 * comfort, we increase the safety margin by
		 * intentionally dropping the next tick(s).  We do NOT
		 * update itm.next because that would force us to call
		 * xtime_update() which in turn would let our clock run
		 * too fast (with the potentially devastating effect
		 * of losing monotony of time).
		 */
		while (!time_after(new_itm, ia64_get_itc() + local_cpu_data->itm_delta/2))
			new_itm += local_cpu_data->itm_delta;
		ia64_set_itm(new_itm);
		/* double check, in case we got hit by a (slow) PMI: */
	} while (time_after_eq(ia64_get_itc(), new_itm));
	return IRQ_HANDLED;
}

/*
 * Encapsulate access to the itm structure for SMP.
 */
void
ia64_cpu_local_tick (void)
{
	int cpu = smp_processor_id();
	unsigned long shift = 0, delta;

	/* arrange for the cycle counter to generate a timer interrupt: */
	ia64_set_itv(IA64_TIMER_VECTOR);

	delta = local_cpu_data->itm_delta;
	/*
	 * Stagger the timer tick for each CPU so they don't occur all at (almost) the
	 * same time:
	 */
	if (cpu) {
		unsigned long hi = 1UL << ia64_fls(cpu);
		shift = (2*(cpu - hi) + 1) * delta/hi/2;
	}
	local_cpu_data->itm_next = ia64_get_itc() + delta + shift;
	ia64_set_itm(local_cpu_data->itm_next);
}

static int nojitter;

static int __init nojitter_setup(char *str)
{
	nojitter = 1;
	printk("Jitter checking for ITC timers disabled\n");
	return 1;
}

__setup("nojitter", nojitter_setup);


void ia64_init_itm(void)
{
	unsigned long platform_base_freq, itc_freq;
	struct pal_freq_ratio itc_ratio, proc_ratio;
	long status, platform_base_drift, itc_drift;

	/*
	 * According to SAL v2.6, we need to use a SAL call to determine the platform base
	 * frequency and then a PAL call to determine the frequency ratio between the ITC
	 * and the base frequency.
	 */
	status = ia64_sal_freq_base(SAL_FREQ_BASE_PLATFORM,
				    &platform_base_freq, &platform_base_drift);
	if (status != 0) {
		printk(KERN_ERR "SAL_FREQ_BASE_PLATFORM failed: %s\n", ia64_sal_strerror(status));
	} else {
		status = ia64_pal_freq_ratios(&proc_ratio, NULL, &itc_ratio);
		if (status != 0)
			printk(KERN_ERR "PAL_FREQ_RATIOS failed with status=%ld\n", status);
	}
	if (status != 0) {
		/* invent "random" values */
		printk(KERN_ERR
		       "SAL/PAL failed to obtain frequency info---inventing reasonable values\n");
		platform_base_freq = 100000000;
		platform_base_drift = -1;	/* no drift info */
		itc_ratio.num = 3;
		itc_ratio.den = 1;
	}
	if (platform_base_freq < 40000000) {
		printk(KERN_ERR "Platform base frequency %lu bogus---resetting to 75MHz!\n",
		       platform_base_freq);
		platform_base_freq = 75000000;
		platform_base_drift = -1;
	}
	if (!proc_ratio.den)
		proc_ratio.den = 1;	/* avoid division by zero */
	if (!itc_ratio.den)
		itc_ratio.den = 1;	/* avoid division by zero */

	itc_freq = (platform_base_freq*itc_ratio.num)/itc_ratio.den;

	local_cpu_data->itm_delta = (itc_freq + HZ/2) / HZ;
	printk(KERN_DEBUG "CPU %d: base freq=%lu.%03luMHz, ITC ratio=%u/%u, "
	       "ITC freq=%lu.%03luMHz", smp_processor_id(),
	       platform_base_freq / 1000000, (platform_base_freq / 1000) % 1000,
	       itc_ratio.num, itc_ratio.den, itc_freq / 1000000, (itc_freq / 1000) % 1000);

	if (platform_base_drift != -1) {
		itc_drift = platform_base_drift*itc_ratio.num/itc_ratio.den;
		printk("+/-%ldppm\n", itc_drift);
	} else {
		itc_drift = -1;
		printk("\n");
	}

	local_cpu_data->proc_freq = (platform_base_freq*proc_ratio.num)/proc_ratio.den;
	local_cpu_data->itc_freq = itc_freq;
	local_cpu_data->cyc_per_usec = (itc_freq + USEC_PER_SEC/2) / USEC_PER_SEC;
	local_cpu_data->nsec_per_cyc = ((NSEC_PER_SEC<<IA64_NSEC_PER_CYC_SHIFT)
					+ itc_freq/2)/itc_freq;

	if (!(sal_platform_features & IA64_SAL_PLATFORM_FEATURE_ITC_DRIFT)) {
#ifdef CONFIG_SMP
		/* On IA64 in an SMP configuration ITCs are never accurately synchronized.
		 * Jitter compensation requires a cmpxchg which may limit
		 * the scalability of the syscalls for retrieving time.
		 * The ITC synchronization is usually successful to within a few
		 * ITC ticks but this is not a sure thing. If you need to improve
		 * timer performance in SMP situations then boot the kernel with the
		 * "nojitter" option. However, doing so may result in time fluctuating (maybe
		 * even going backward) if the ITC offsets between the individual CPUs
		 * are too large.
		 */
		if (!nojitter)
			itc_jitter_data.itc_jitter = 1;
#endif
	} else
		/*
		 * ITC is drifty and we have not synchronized the ITCs in smpboot.c.
		 * ITC values may fluctuate significantly between processors.
		 * Clock should not be used for hrtimers. Mark itc as only
		 * useful for boot and testing.
		 *
		 * Note that jitter compensation is off! There is no point of
		 * synchronizing ITCs since they may be large differentials
		 * that change over time.
		 *
		 * The only way to fix this would be to repeatedly sync the
		 * ITCs. Until that time we have to avoid ITC.
		 */
		clocksource_itc.rating = 50;

	/* avoid softlock up message when cpu is unplug and plugged again. */
	touch_softlockup_watchdog();

	/* Setup the CPU local timer tick */
	ia64_cpu_local_tick();

	if (!itc_clocksource) {
		clocksource_register_hz(&clocksource_itc,
						local_cpu_data->itc_freq);
		itc_clocksource = &clocksource_itc;
	}
}

static u64 itc_get_cycles(struct clocksource *cs)
{
	unsigned long lcycle, now, ret;

	if (!itc_jitter_data.itc_jitter)
		return get_cycles();

	lcycle = itc_jitter_data.itc_lastcycle;
	now = get_cycles();
	if (lcycle && time_after(lcycle, now))
		return lcycle;

	/*
	 * Keep track of the last timer value returned.
	 * In an SMP environment, you could lose out in contention of
	 * cmpxchg. If so, your cmpxchg returns new value which the
	 * winner of contention updated to. Use the new value instead.
	 */
	ret = cmpxchg(&itc_jitter_data.itc_lastcycle, lcycle, now);
	if (unlikely(ret != lcycle))
		return ret;

	return now;
}

void read_persistent_clock64(struct timespec64 *ts)
{
	efi_gettimeofday(ts);
}

void __init
time_init (void)
{
	register_percpu_irq(IA64_TIMER_VECTOR, timer_interrupt, IRQF_IRQPOLL,
			    "timer");
	ia64_init_itm();
}

/*
 * Generic udelay assumes that if preemption is allowed and the thread
 * migrates to another CPU, that the ITC values are synchronized across
 * all CPUs.
 */
static void
ia64_itc_udelay (unsigned long usecs)
{
	unsigned long start = ia64_get_itc();
	unsigned long end = start + usecs*local_cpu_data->cyc_per_usec;

	while (time_before(ia64_get_itc(), end))
		cpu_relax();
}

void (*ia64_udelay)(unsigned long usecs) = &ia64_itc_udelay;

void
udelay (unsigned long usecs)
{
	(*ia64_udelay)(usecs);
}
EXPORT_SYMBOL(udelay);

/* IA64 doesn't cache the timezone */
void update_vsyscall_tz(void)
{
}

void update_vsyscall(struct timekeeper *tk)
{
	write_seqcount_begin(&fsyscall_gtod_data.seq);

	/* copy vsyscall data */
	fsyscall_gtod_data.clk_mask = tk->tkr_mono.mask;
	fsyscall_gtod_data.clk_mult = tk->tkr_mono.mult;
	fsyscall_gtod_data.clk_shift = tk->tkr_mono.shift;
	fsyscall_gtod_data.clk_fsys_mmio = tk->tkr_mono.clock->archdata.fsys_mmio;
	fsyscall_gtod_data.clk_cycle_last = tk->tkr_mono.cycle_last;

	fsyscall_gtod_data.wall_time.sec = tk->xtime_sec;
	fsyscall_gtod_data.wall_time.snsec = tk->tkr_mono.xtime_nsec;

	fsyscall_gtod_data.monotonic_time.sec = tk->xtime_sec
					      + tk->wall_to_monotonic.tv_sec;
	fsyscall_gtod_data.monotonic_time.snsec = tk->tkr_mono.xtime_nsec
						+ ((u64)tk->wall_to_monotonic.tv_nsec
							<< tk->tkr_mono.shift);

	/* normalize */
	while (fsyscall_gtod_data.monotonic_time.snsec >=
					(((u64)NSEC_PER_SEC) << tk->tkr_mono.shift)) {
		fsyscall_gtod_data.monotonic_time.snsec -=
					((u64)NSEC_PER_SEC) << tk->tkr_mono.shift;
		fsyscall_gtod_data.monotonic_time.sec++;
	}

	write_seqcount_end(&fsyscall_gtod_data.seq);
}

