// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright(C) 2005-2006, Thomas Gleixner <tglx@linutronix.de>
 *  Copyright(C) 2005-2007, Red Hat, Inc., Ingo Molnar
 *  Copyright(C) 2006-2007  Timesys Corp., Thomas Gleixner
 *
 *  No idle tick implementation for low and high resolution timers
 *
 *  Started by: Thomas Gleixner and Ingo Molnar
 */
#include <linux/cpu.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/percpu.h>
#include <linux/nmi.h>
#include <linux/profile.h>
#include <linux/sched/signal.h>
#include <linux/sched/clock.h>
#include <linux/sched/stat.h>
#include <linux/sched/nohz.h>
#include <linux/sched/loadavg.h>
#include <linux/module.h>
#include <linux/irq_work.h>
#include <linux/posix-timers.h>
#include <linux/context_tracking.h>
#include <linux/mm.h>

#include <asm/irq_regs.h>

#include "tick-internal.h"

#include <trace/events/timer.h>

/*
 * Per-CPU nohz control structure
 */
static DEFINE_PER_CPU(struct tick_sched, tick_cpu_sched);

struct tick_sched *tick_get_tick_sched(int cpu)
{
	return &per_cpu(tick_cpu_sched, cpu);
}

#if defined(CONFIG_NO_HZ_COMMON) || defined(CONFIG_HIGH_RES_TIMERS)
/*
 * The time, when the last jiffy update happened. Write access must hold
 * jiffies_lock and jiffies_seq. tick_nohz_next_event() needs to get a
 * consistent view of jiffies and last_jiffies_update.
 */
static ktime_t last_jiffies_update;

/*
 * Must be called with interrupts disabled !
 */
static void tick_do_update_jiffies64(ktime_t now)
{
	unsigned long ticks = 1;
	ktime_t delta, nextp;

	/*
	 * 64bit can do a quick check without holding jiffies lock and
	 * without looking at the sequence count. The smp_load_acquire()
	 * pairs with the update done later in this function.
	 *
	 * 32bit cannot do that because the store of tick_next_period
	 * consists of two 32bit stores and the first store could move it
	 * to a random point in the future.
	 */
	if (IS_ENABLED(CONFIG_64BIT)) {
		if (ktime_before(now, smp_load_acquire(&tick_next_period)))
			return;
	} else {
		unsigned int seq;

		/*
		 * Avoid contention on jiffies_lock and protect the quick
		 * check with the sequence count.
		 */
		do {
			seq = read_seqcount_begin(&jiffies_seq);
			nextp = tick_next_period;
		} while (read_seqcount_retry(&jiffies_seq, seq));

		if (ktime_before(now, nextp))
			return;
	}

	/* Quick check failed, i.e. update is required. */
	raw_spin_lock(&jiffies_lock);
	/*
	 * Reevaluate with the lock held. Another CPU might have done the
	 * update already.
	 */
	if (ktime_before(now, tick_next_period)) {
		raw_spin_unlock(&jiffies_lock);
		return;
	}

	write_seqcount_begin(&jiffies_seq);

	delta = ktime_sub(now, tick_next_period);
	if (unlikely(delta >= TICK_NSEC)) {
		/* Slow path for long idle sleep times */
		s64 incr = TICK_NSEC;

		ticks += ktime_divns(delta, incr);

		last_jiffies_update = ktime_add_ns(last_jiffies_update,
						   incr * ticks);
	} else {
		last_jiffies_update = ktime_add_ns(last_jiffies_update,
						   TICK_NSEC);
	}

	/* Advance jiffies to complete the jiffies_seq protected job */
	jiffies_64 += ticks;

	/*
	 * Keep the tick_next_period variable up to date.
	 */
	nextp = ktime_add_ns(last_jiffies_update, TICK_NSEC);

	if (IS_ENABLED(CONFIG_64BIT)) {
		/*
		 * Pairs with smp_load_acquire() in the lockless quick
		 * check above and ensures that the update to jiffies_64 is
		 * not reordered vs. the store to tick_next_period, neither
		 * by the compiler nor by the CPU.
		 */
		smp_store_release(&tick_next_period, nextp);
	} else {
		/*
		 * A plain store is good enough on 32bit as the quick check
		 * above is protected by the sequence count.
		 */
		tick_next_period = nextp;
	}

	/*
	 * Release the sequence count. calc_global_load() below is not
	 * protected by it, but jiffies_lock needs to be held to prevent
	 * concurrent invocations.
	 */
	write_seqcount_end(&jiffies_seq);

	calc_global_load();

	raw_spin_unlock(&jiffies_lock);
	update_wall_time();
}

/*
 * Initialize and return retrieve the jiffies update.
 */
static ktime_t tick_init_jiffy_update(void)
{
	ktime_t period;

	raw_spin_lock(&jiffies_lock);
	write_seqcount_begin(&jiffies_seq);
	/* Did we start the jiffies update yet ? */
	if (last_jiffies_update == 0)
		last_jiffies_update = tick_next_period;
	period = last_jiffies_update;
	write_seqcount_end(&jiffies_seq);
	raw_spin_unlock(&jiffies_lock);
	return period;
}

static void tick_sched_do_timer(struct tick_sched *ts, ktime_t now)
{
	int cpu = smp_processor_id();

#ifdef CONFIG_NO_HZ_COMMON
	/*
	 * Check if the do_timer duty was dropped. We don't care about
	 * concurrency: This happens only when the CPU in charge went
	 * into a long sleep. If two CPUs happen to assign themselves to
	 * this duty, then the jiffies update is still serialized by
	 * jiffies_lock.
	 *
	 * If nohz_full is enabled, this should not happen because the
	 * tick_do_timer_cpu never relinquishes.
	 */
	if (unlikely(tick_do_timer_cpu == TICK_DO_TIMER_NONE)) {
#ifdef CONFIG_NO_HZ_FULL
		WARN_ON(tick_nohz_full_running);
#endif
		tick_do_timer_cpu = cpu;
	}
#endif

	/* Check, if the jiffies need an update */
	if (tick_do_timer_cpu == cpu)
		tick_do_update_jiffies64(now);

	if (ts->inidle)
		ts->got_idle_tick = 1;
}

static void tick_sched_handle(struct tick_sched *ts, struct pt_regs *regs)
{
#ifdef CONFIG_NO_HZ_COMMON
	/*
	 * When we are idle and the tick is stopped, we have to touch
	 * the watchdog as we might not schedule for a really long
	 * time. This happens on complete idle SMP systems while
	 * waiting on the login prompt. We also increment the "start of
	 * idle" jiffy stamp so the idle accounting adjustment we do
	 * when we go busy again does not account too much ticks.
	 */
	if (ts->tick_stopped) {
		touch_softlockup_watchdog_sched();
		if (is_idle_task(current))
			ts->idle_jiffies++;
		/*
		 * In case the current tick fired too early past its expected
		 * expiration, make sure we don't bypass the next clock reprogramming
		 * to the same deadline.
		 */
		ts->next_tick = 0;
	}
#endif
	update_process_times(user_mode(regs));
	profile_tick(CPU_PROFILING);
}
#endif

#ifdef CONFIG_NO_HZ_FULL
cpumask_var_t tick_nohz_full_mask;
EXPORT_SYMBOL_GPL(tick_nohz_full_mask);
bool tick_nohz_full_running;
EXPORT_SYMBOL_GPL(tick_nohz_full_running);
static atomic_t tick_dep_mask;

static bool check_tick_dependency(atomic_t *dep)
{
	int val = atomic_read(dep);

	if (val & TICK_DEP_MASK_POSIX_TIMER) {
		trace_tick_stop(0, TICK_DEP_MASK_POSIX_TIMER);
		return true;
	}

	if (val & TICK_DEP_MASK_PERF_EVENTS) {
		trace_tick_stop(0, TICK_DEP_MASK_PERF_EVENTS);
		return true;
	}

	if (val & TICK_DEP_MASK_SCHED) {
		trace_tick_stop(0, TICK_DEP_MASK_SCHED);
		return true;
	}

	if (val & TICK_DEP_MASK_CLOCK_UNSTABLE) {
		trace_tick_stop(0, TICK_DEP_MASK_CLOCK_UNSTABLE);
		return true;
	}

	if (val & TICK_DEP_MASK_RCU) {
		trace_tick_stop(0, TICK_DEP_MASK_RCU);
		return true;
	}

	return false;
}

static bool can_stop_full_tick(int cpu, struct tick_sched *ts)
{
	lockdep_assert_irqs_disabled();

	if (unlikely(!cpu_online(cpu)))
		return false;

	if (check_tick_dependency(&tick_dep_mask))
		return false;

	if (check_tick_dependency(&ts->tick_dep_mask))
		return false;

	if (check_tick_dependency(&current->tick_dep_mask))
		return false;

	if (check_tick_dependency(&current->signal->tick_dep_mask))
		return false;

	return true;
}

static void nohz_full_kick_func(struct irq_work *work)
{
	/* Empty, the tick restart happens on tick_nohz_irq_exit() */
}

static DEFINE_PER_CPU(struct irq_work, nohz_full_kick_work) =
	IRQ_WORK_INIT_HARD(nohz_full_kick_func);

/*
 * Kick this CPU if it's full dynticks in order to force it to
 * re-evaluate its dependency on the tick and restart it if necessary.
 * This kick, unlike tick_nohz_full_kick_cpu() and tick_nohz_full_kick_all(),
 * is NMI safe.
 */
static void tick_nohz_full_kick(void)
{
	if (!tick_nohz_full_cpu(smp_processor_id()))
		return;

	irq_work_queue(this_cpu_ptr(&nohz_full_kick_work));
}

/*
 * Kick the CPU if it's full dynticks in order to force it to
 * re-evaluate its dependency on the tick and restart it if necessary.
 */
void tick_nohz_full_kick_cpu(int cpu)
{
	if (!tick_nohz_full_cpu(cpu))
		return;

	irq_work_queue_on(&per_cpu(nohz_full_kick_work, cpu), cpu);
}

/*
 * Kick all full dynticks CPUs in order to force these to re-evaluate
 * their dependency on the tick and restart it if necessary.
 */
static void tick_nohz_full_kick_all(void)
{
	int cpu;

	if (!tick_nohz_full_running)
		return;

	preempt_disable();
	for_each_cpu_and(cpu, tick_nohz_full_mask, cpu_online_mask)
		tick_nohz_full_kick_cpu(cpu);
	preempt_enable();
}

static void tick_nohz_dep_set_all(atomic_t *dep,
				  enum tick_dep_bits bit)
{
	int prev;

	prev = atomic_fetch_or(BIT(bit), dep);
	if (!prev)
		tick_nohz_full_kick_all();
}

/*
 * Set a global tick dependency. Used by perf events that rely on freq and
 * by unstable clock.
 */
void tick_nohz_dep_set(enum tick_dep_bits bit)
{
	tick_nohz_dep_set_all(&tick_dep_mask, bit);
}

void tick_nohz_dep_clear(enum tick_dep_bits bit)
{
	atomic_andnot(BIT(bit), &tick_dep_mask);
}

/*
 * Set per-CPU tick dependency. Used by scheduler and perf events in order to
 * manage events throttling.
 */
void tick_nohz_dep_set_cpu(int cpu, enum tick_dep_bits bit)
{
	int prev;
	struct tick_sched *ts;

	ts = per_cpu_ptr(&tick_cpu_sched, cpu);

	prev = atomic_fetch_or(BIT(bit), &ts->tick_dep_mask);
	if (!prev) {
		preempt_disable();
		/* Perf needs local kick that is NMI safe */
		if (cpu == smp_processor_id()) {
			tick_nohz_full_kick();
		} else {
			/* Remote irq work not NMI-safe */
			if (!WARN_ON_ONCE(in_nmi()))
				tick_nohz_full_kick_cpu(cpu);
		}
		preempt_enable();
	}
}
EXPORT_SYMBOL_GPL(tick_nohz_dep_set_cpu);

void tick_nohz_dep_clear_cpu(int cpu, enum tick_dep_bits bit)
{
	struct tick_sched *ts = per_cpu_ptr(&tick_cpu_sched, cpu);

	atomic_andnot(BIT(bit), &ts->tick_dep_mask);
}
EXPORT_SYMBOL_GPL(tick_nohz_dep_clear_cpu);

/*
 * Set a per-task tick dependency. RCU need this. Also posix CPU timers
 * in order to elapse per task timers.
 */
void tick_nohz_dep_set_task(struct task_struct *tsk, enum tick_dep_bits bit)
{
	if (!atomic_fetch_or(BIT(bit), &tsk->tick_dep_mask)) {
		if (tsk == current) {
			preempt_disable();
			tick_nohz_full_kick();
			preempt_enable();
		} else {
			/*
			 * Some future tick_nohz_full_kick_task()
			 * should optimize this.
			 */
			tick_nohz_full_kick_all();
		}
	}
}
EXPORT_SYMBOL_GPL(tick_nohz_dep_set_task);

void tick_nohz_dep_clear_task(struct task_struct *tsk, enum tick_dep_bits bit)
{
	atomic_andnot(BIT(bit), &tsk->tick_dep_mask);
}
EXPORT_SYMBOL_GPL(tick_nohz_dep_clear_task);

/*
 * Set a per-taskgroup tick dependency. Posix CPU timers need this in order to elapse
 * per process timers.
 */
void tick_nohz_dep_set_signal(struct signal_struct *sig, enum tick_dep_bits bit)
{
	tick_nohz_dep_set_all(&sig->tick_dep_mask, bit);
}

void tick_nohz_dep_clear_signal(struct signal_struct *sig, enum tick_dep_bits bit)
{
	atomic_andnot(BIT(bit), &sig->tick_dep_mask);
}

/*
 * Re-evaluate the need for the tick as we switch the current task.
 * It might need the tick due to per task/process properties:
 * perf events, posix CPU timers, ...
 */
void __tick_nohz_task_switch(void)
{
	unsigned long flags;
	struct tick_sched *ts;

	local_irq_save(flags);

	if (!tick_nohz_full_cpu(smp_processor_id()))
		goto out;

	ts = this_cpu_ptr(&tick_cpu_sched);

	if (ts->tick_stopped) {
		if (atomic_read(&current->tick_dep_mask) ||
		    atomic_read(&current->signal->tick_dep_mask))
			tick_nohz_full_kick();
	}
out:
	local_irq_restore(flags);
}

/* Get the boot-time nohz CPU list from the kernel parameters. */
void __init tick_nohz_full_setup(cpumask_var_t cpumask)
{
	alloc_bootmem_cpumask_var(&tick_nohz_full_mask);
	cpumask_copy(tick_nohz_full_mask, cpumask);
	tick_nohz_full_running = true;
}
EXPORT_SYMBOL_GPL(tick_nohz_full_setup);

static int tick_nohz_cpu_down(unsigned int cpu)
{
	/*
	 * The tick_do_timer_cpu CPU handles housekeeping duty (unbound
	 * timers, workqueues, timekeeping, ...) on behalf of full dynticks
	 * CPUs. It must remain online when nohz full is enabled.
	 */
	if (tick_nohz_full_running && tick_do_timer_cpu == cpu)
		return -EBUSY;
	return 0;
}

void __init tick_nohz_init(void)
{
	int cpu, ret;

	if (!tick_nohz_full_running)
		return;

	/*
	 * Full dynticks uses irq work to drive the tick rescheduling on safe
	 * locking contexts. But then we need irq work to raise its own
	 * interrupts to avoid circular dependency on the tick
	 */
	if (!arch_irq_work_has_interrupt()) {
		pr_warn("NO_HZ: Can't run full dynticks because arch doesn't support irq work self-IPIs\n");
		cpumask_clear(tick_nohz_full_mask);
		tick_nohz_full_running = false;
		return;
	}

	if (IS_ENABLED(CONFIG_PM_SLEEP_SMP) &&
			!IS_ENABLED(CONFIG_PM_SLEEP_SMP_NONZERO_CPU)) {
		cpu = smp_processor_id();

		if (cpumask_test_cpu(cpu, tick_nohz_full_mask)) {
			pr_warn("NO_HZ: Clearing %d from nohz_full range "
				"for timekeeping\n", cpu);
			cpumask_clear_cpu(cpu, tick_nohz_full_mask);
		}
	}

	for_each_cpu(cpu, tick_nohz_full_mask)
		context_tracking_cpu_set(cpu);

	ret = cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN,
					"kernel/nohz:predown", NULL,
					tick_nohz_cpu_down);
	WARN_ON(ret < 0);
	pr_info("NO_HZ: Full dynticks CPUs: %*pbl.\n",
		cpumask_pr_args(tick_nohz_full_mask));
}
#endif

/*
 * NOHZ - aka dynamic tick functionality
 */
#ifdef CONFIG_NO_HZ_COMMON
/*
 * NO HZ enabled ?
 */
bool tick_nohz_enabled __read_mostly  = true;
unsigned long tick_nohz_active  __read_mostly;
/*
 * Enable / Disable tickless mode
 */
static int __init setup_tick_nohz(char *str)
{
	return (kstrtobool(str, &tick_nohz_enabled) == 0);
}

__setup("nohz=", setup_tick_nohz);

bool tick_nohz_tick_stopped(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);

	return ts->tick_stopped;
}

bool tick_nohz_tick_stopped_cpu(int cpu)
{
	struct tick_sched *ts = per_cpu_ptr(&tick_cpu_sched, cpu);

	return ts->tick_stopped;
}

/**
 * tick_nohz_update_jiffies - update jiffies when idle was interrupted
 *
 * Called from interrupt entry when the CPU was idle
 *
 * In case the sched_tick was stopped on this CPU, we have to check if jiffies
 * must be updated. Otherwise an interrupt handler could use a stale jiffy
 * value. We do this unconditionally on any CPU, as we don't know whether the
 * CPU, which has the update task assigned is in a long sleep.
 */
static void tick_nohz_update_jiffies(ktime_t now)
{
	unsigned long flags;

	__this_cpu_write(tick_cpu_sched.idle_waketime, now);

	local_irq_save(flags);
	tick_do_update_jiffies64(now);
	local_irq_restore(flags);

	touch_softlockup_watchdog_sched();
}

/*
 * Updates the per-CPU time idle statistics counters
 */
static void
update_ts_time_stats(int cpu, struct tick_sched *ts, ktime_t now, u64 *last_update_time)
{
	ktime_t delta;

	if (ts->idle_active) {
		delta = ktime_sub(now, ts->idle_entrytime);
		if (nr_iowait_cpu(cpu) > 0)
			ts->iowait_sleeptime = ktime_add(ts->iowait_sleeptime, delta);
		else
			ts->idle_sleeptime = ktime_add(ts->idle_sleeptime, delta);
		ts->idle_entrytime = now;
	}

	if (last_update_time)
		*last_update_time = ktime_to_us(now);

}

static void tick_nohz_stop_idle(struct tick_sched *ts, ktime_t now)
{
	update_ts_time_stats(smp_processor_id(), ts, now, NULL);
	ts->idle_active = 0;

	sched_clock_idle_wakeup_event();
}

static void tick_nohz_start_idle(struct tick_sched *ts)
{
	ts->idle_entrytime = ktime_get();
	ts->idle_active = 1;
	sched_clock_idle_sleep_event();
}

/**
 * get_cpu_idle_time_us - get the total idle time of a CPU
 * @cpu: CPU number to query
 * @last_update_time: variable to store update time in. Do not update
 * counters if NULL.
 *
 * Return the cumulative idle time (since boot) for a given
 * CPU, in microseconds.
 *
 * This time is measured via accounting rather than sampling,
 * and is as accurate as ktime_get() is.
 *
 * This function returns -1 if NOHZ is not enabled.
 */
u64 get_cpu_idle_time_us(int cpu, u64 *last_update_time)
{
	struct tick_sched *ts = &per_cpu(tick_cpu_sched, cpu);
	ktime_t now, idle;

	if (!tick_nohz_active)
		return -1;

	now = ktime_get();
	if (last_update_time) {
		update_ts_time_stats(cpu, ts, now, last_update_time);
		idle = ts->idle_sleeptime;
	} else {
		if (ts->idle_active && !nr_iowait_cpu(cpu)) {
			ktime_t delta = ktime_sub(now, ts->idle_entrytime);

			idle = ktime_add(ts->idle_sleeptime, delta);
		} else {
			idle = ts->idle_sleeptime;
		}
	}

	return ktime_to_us(idle);

}
EXPORT_SYMBOL_GPL(get_cpu_idle_time_us);

/**
 * get_cpu_iowait_time_us - get the total iowait time of a CPU
 * @cpu: CPU number to query
 * @last_update_time: variable to store update time in. Do not update
 * counters if NULL.
 *
 * Return the cumulative iowait time (since boot) for a given
 * CPU, in microseconds.
 *
 * This time is measured via accounting rather than sampling,
 * and is as accurate as ktime_get() is.
 *
 * This function returns -1 if NOHZ is not enabled.
 */
u64 get_cpu_iowait_time_us(int cpu, u64 *last_update_time)
{
	struct tick_sched *ts = &per_cpu(tick_cpu_sched, cpu);
	ktime_t now, iowait;

	if (!tick_nohz_active)
		return -1;

	now = ktime_get();
	if (last_update_time) {
		update_ts_time_stats(cpu, ts, now, last_update_time);
		iowait = ts->iowait_sleeptime;
	} else {
		if (ts->idle_active && nr_iowait_cpu(cpu) > 0) {
			ktime_t delta = ktime_sub(now, ts->idle_entrytime);

			iowait = ktime_add(ts->iowait_sleeptime, delta);
		} else {
			iowait = ts->iowait_sleeptime;
		}
	}

	return ktime_to_us(iowait);
}
EXPORT_SYMBOL_GPL(get_cpu_iowait_time_us);

static void tick_nohz_restart(struct tick_sched *ts, ktime_t now)
{
	hrtimer_cancel(&ts->sched_timer);
	hrtimer_set_expires(&ts->sched_timer, ts->last_tick);

	/* Forward the time to expire in the future */
	hrtimer_forward(&ts->sched_timer, now, TICK_NSEC);

	if (ts->nohz_mode == NOHZ_MODE_HIGHRES) {
		hrtimer_start_expires(&ts->sched_timer,
				      HRTIMER_MODE_ABS_PINNED_HARD);
	} else {
		tick_program_event(hrtimer_get_expires(&ts->sched_timer), 1);
	}

	/*
	 * Reset to make sure next tick stop doesn't get fooled by past
	 * cached clock deadline.
	 */
	ts->next_tick = 0;
}

static inline bool local_timer_softirq_pending(void)
{
	return local_softirq_pending() & BIT(TIMER_SOFTIRQ);
}

static ktime_t tick_nohz_next_event(struct tick_sched *ts, int cpu)
{
	u64 basemono, next_tick, next_tmr, next_rcu, delta, expires;
	unsigned long basejiff;
	unsigned int seq;

	/* Read jiffies and the time when jiffies were updated last */
	do {
		seq = read_seqcount_begin(&jiffies_seq);
		basemono = last_jiffies_update;
		basejiff = jiffies;
	} while (read_seqcount_retry(&jiffies_seq, seq));
	ts->last_jiffies = basejiff;
	ts->timer_expires_base = basemono;

	/*
	 * Keep the periodic tick, when RCU, architecture or irq_work
	 * requests it.
	 * Aside of that check whether the local timer softirq is
	 * pending. If so its a bad idea to call get_next_timer_interrupt()
	 * because there is an already expired timer, so it will request
	 * immediate expiry, which rearms the hardware timer with a
	 * minimal delta which brings us back to this place
	 * immediately. Lather, rinse and repeat...
	 */
	if (rcu_needs_cpu(basemono, &next_rcu) || arch_needs_cpu() ||
	    irq_work_needs_cpu() || local_timer_softirq_pending()) {
		next_tick = basemono + TICK_NSEC;
	} else {
		/*
		 * Get the next pending timer. If high resolution
		 * timers are enabled this only takes the timer wheel
		 * timers into account. If high resolution timers are
		 * disabled this also looks at the next expiring
		 * hrtimer.
		 */
		next_tmr = get_next_timer_interrupt(basejiff, basemono);
		ts->next_timer = next_tmr;
		/* Take the next rcu event into account */
		next_tick = next_rcu < next_tmr ? next_rcu : next_tmr;
	}

	/*
	 * If the tick is due in the next period, keep it ticking or
	 * force prod the timer.
	 */
	delta = next_tick - basemono;
	if (delta <= (u64)TICK_NSEC) {
		/*
		 * Tell the timer code that the base is not idle, i.e. undo
		 * the effect of get_next_timer_interrupt():
		 */
		timer_clear_idle();
		/*
		 * We've not stopped the tick yet, and there's a timer in the
		 * next period, so no point in stopping it either, bail.
		 */
		if (!ts->tick_stopped) {
			ts->timer_expires = 0;
			goto out;
		}
	}

	/*
	 * If this CPU is the one which had the do_timer() duty last, we limit
	 * the sleep time to the timekeeping max_deferment value.
	 * Otherwise we can sleep as long as we want.
	 */
	delta = timekeeping_max_deferment();
	if (cpu != tick_do_timer_cpu &&
	    (tick_do_timer_cpu != TICK_DO_TIMER_NONE || !ts->do_timer_last))
		delta = KTIME_MAX;

	/* Calculate the next expiry time */
	if (delta < (KTIME_MAX - basemono))
		expires = basemono + delta;
	else
		expires = KTIME_MAX;

	ts->timer_expires = min_t(u64, expires, next_tick);

out:
	return ts->timer_expires;
}

static void tick_nohz_stop_tick(struct tick_sched *ts, int cpu)
{
	struct clock_event_device *dev = __this_cpu_read(tick_cpu_device.evtdev);
	u64 basemono = ts->timer_expires_base;
	u64 expires = ts->timer_expires;
	ktime_t tick = expires;

	/* Make sure we won't be trying to stop it twice in a row. */
	ts->timer_expires_base = 0;

	/*
	 * If this CPU is the one which updates jiffies, then give up
	 * the assignment and let it be taken by the CPU which runs
	 * the tick timer next, which might be this CPU as well. If we
	 * don't drop this here the jiffies might be stale and
	 * do_timer() never invoked. Keep track of the fact that it
	 * was the one which had the do_timer() duty last.
	 */
	if (cpu == tick_do_timer_cpu) {
		tick_do_timer_cpu = TICK_DO_TIMER_NONE;
		ts->do_timer_last = 1;
	} else if (tick_do_timer_cpu != TICK_DO_TIMER_NONE) {
		ts->do_timer_last = 0;
	}

	/* Skip reprogram of event if its not changed */
	if (ts->tick_stopped && (expires == ts->next_tick)) {
		/* Sanity check: make sure clockevent is actually programmed */
		if (tick == KTIME_MAX || ts->next_tick == hrtimer_get_expires(&ts->sched_timer))
			return;

		WARN_ON_ONCE(1);
		printk_once("basemono: %llu ts->next_tick: %llu dev->next_event: %llu timer->active: %d timer->expires: %llu\n",
			    basemono, ts->next_tick, dev->next_event,
			    hrtimer_active(&ts->sched_timer), hrtimer_get_expires(&ts->sched_timer));
	}

	/*
	 * nohz_stop_sched_tick can be called several times before
	 * the nohz_restart_sched_tick is called. This happens when
	 * interrupts arrive which do not cause a reschedule. In the
	 * first call we save the current tick time, so we can restart
	 * the scheduler tick in nohz_restart_sched_tick.
	 */
	if (!ts->tick_stopped) {
		calc_load_nohz_start();
		quiet_vmstat();

		ts->last_tick = hrtimer_get_expires(&ts->sched_timer);
		ts->tick_stopped = 1;
		trace_tick_stop(1, TICK_DEP_MASK_NONE);
	}

	ts->next_tick = tick;

	/*
	 * If the expiration time == KTIME_MAX, then we simply stop
	 * the tick timer.
	 */
	if (unlikely(expires == KTIME_MAX)) {
		if (ts->nohz_mode == NOHZ_MODE_HIGHRES)
			hrtimer_cancel(&ts->sched_timer);
		return;
	}

	if (ts->nohz_mode == NOHZ_MODE_HIGHRES) {
		hrtimer_start(&ts->sched_timer, tick,
			      HRTIMER_MODE_ABS_PINNED_HARD);
	} else {
		hrtimer_set_expires(&ts->sched_timer, tick);
		tick_program_event(tick, 1);
	}
}

static void tick_nohz_retain_tick(struct tick_sched *ts)
{
	ts->timer_expires_base = 0;
}

#ifdef CONFIG_NO_HZ_FULL
static void tick_nohz_stop_sched_tick(struct tick_sched *ts, int cpu)
{
	if (tick_nohz_next_event(ts, cpu))
		tick_nohz_stop_tick(ts, cpu);
	else
		tick_nohz_retain_tick(ts);
}
#endif /* CONFIG_NO_HZ_FULL */

static void tick_nohz_restart_sched_tick(struct tick_sched *ts, ktime_t now)
{
	/* Update jiffies first */
	tick_do_update_jiffies64(now);

	/*
	 * Clear the timer idle flag, so we avoid IPIs on remote queueing and
	 * the clock forward checks in the enqueue path:
	 */
	timer_clear_idle();

	calc_load_nohz_stop();
	touch_softlockup_watchdog_sched();
	/*
	 * Cancel the scheduled timer and restore the tick
	 */
	ts->tick_stopped  = 0;
	ts->idle_exittime = now;

	tick_nohz_restart(ts, now);
}

static void tick_nohz_full_update_tick(struct tick_sched *ts)
{
#ifdef CONFIG_NO_HZ_FULL
	int cpu = smp_processor_id();

	if (!tick_nohz_full_cpu(cpu))
		return;

	if (!ts->tick_stopped && ts->nohz_mode == NOHZ_MODE_INACTIVE)
		return;

	if (can_stop_full_tick(cpu, ts))
		tick_nohz_stop_sched_tick(ts, cpu);
	else if (ts->tick_stopped)
		tick_nohz_restart_sched_tick(ts, ktime_get());
#endif
}

static bool can_stop_idle_tick(int cpu, struct tick_sched *ts)
{
	/*
	 * If this CPU is offline and it is the one which updates
	 * jiffies, then give up the assignment and let it be taken by
	 * the CPU which runs the tick timer next. If we don't drop
	 * this here the jiffies might be stale and do_timer() never
	 * invoked.
	 */
	if (unlikely(!cpu_online(cpu))) {
		if (cpu == tick_do_timer_cpu)
			tick_do_timer_cpu = TICK_DO_TIMER_NONE;
		/*
		 * Make sure the CPU doesn't get fooled by obsolete tick
		 * deadline if it comes back online later.
		 */
		ts->next_tick = 0;
		return false;
	}

	if (unlikely(ts->nohz_mode == NOHZ_MODE_INACTIVE))
		return false;

	if (need_resched())
		return false;

	if (unlikely(local_softirq_pending())) {
		static int ratelimit;

		if (ratelimit < 10 && !local_bh_blocked() &&
		    (local_softirq_pending() & SOFTIRQ_STOP_IDLE_MASK)) {
			pr_warn("NOHZ tick-stop error: Non-RCU local softirq work is pending, handler #%02x!!!\n",
				(unsigned int) local_softirq_pending());
			ratelimit++;
		}
		return false;
	}

	if (tick_nohz_full_enabled()) {
		/*
		 * Keep the tick alive to guarantee timekeeping progression
		 * if there are full dynticks CPUs around
		 */
		if (tick_do_timer_cpu == cpu)
			return false;

		/* Should not happen for nohz-full */
		if (WARN_ON_ONCE(tick_do_timer_cpu == TICK_DO_TIMER_NONE))
			return false;
	}

	return true;
}

static void __tick_nohz_idle_stop_tick(struct tick_sched *ts)
{
	ktime_t expires;
	int cpu = smp_processor_id();

	/*
	 * If tick_nohz_get_sleep_length() ran tick_nohz_next_event(), the
	 * tick timer expiration time is known already.
	 */
	if (ts->timer_expires_base)
		expires = ts->timer_expires;
	else if (can_stop_idle_tick(cpu, ts))
		expires = tick_nohz_next_event(ts, cpu);
	else
		return;

	ts->idle_calls++;

	if (expires > 0LL) {
		int was_stopped = ts->tick_stopped;

		tick_nohz_stop_tick(ts, cpu);

		ts->idle_sleeps++;
		ts->idle_expires = expires;

		if (!was_stopped && ts->tick_stopped) {
			ts->idle_jiffies = ts->last_jiffies;
			nohz_balance_enter_idle(cpu);
		}
	} else {
		tick_nohz_retain_tick(ts);
	}
}

/**
 * tick_nohz_idle_stop_tick - stop the idle tick from the idle task
 *
 * When the next event is more than a tick into the future, stop the idle tick
 */
void tick_nohz_idle_stop_tick(void)
{
	__tick_nohz_idle_stop_tick(this_cpu_ptr(&tick_cpu_sched));
}

void tick_nohz_idle_retain_tick(void)
{
	tick_nohz_retain_tick(this_cpu_ptr(&tick_cpu_sched));
	/*
	 * Undo the effect of get_next_timer_interrupt() called from
	 * tick_nohz_next_event().
	 */
	timer_clear_idle();
}

/**
 * tick_nohz_idle_enter - prepare for entering idle on the current CPU
 *
 * Called when we start the idle loop.
 */
void tick_nohz_idle_enter(void)
{
	struct tick_sched *ts;

	lockdep_assert_irqs_enabled();

	local_irq_disable();

	ts = this_cpu_ptr(&tick_cpu_sched);

	WARN_ON_ONCE(ts->timer_expires_base);

	ts->inidle = 1;
	tick_nohz_start_idle(ts);

	local_irq_enable();
}

/**
 * tick_nohz_irq_exit - update next tick event from interrupt exit
 *
 * When an interrupt fires while we are idle and it doesn't cause
 * a reschedule, it may still add, modify or delete a timer, enqueue
 * an RCU callback, etc...
 * So we need to re-calculate and reprogram the next tick event.
 */
void tick_nohz_irq_exit(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);

	if (ts->inidle)
		tick_nohz_start_idle(ts);
	else
		tick_nohz_full_update_tick(ts);
}

/**
 * tick_nohz_idle_got_tick - Check whether or not the tick handler has run
 */
bool tick_nohz_idle_got_tick(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);

	if (ts->got_idle_tick) {
		ts->got_idle_tick = 0;
		return true;
	}
	return false;
}

/**
 * tick_nohz_get_next_hrtimer - return the next expiration time for the hrtimer
 * or the tick, whatever that expires first. Note that, if the tick has been
 * stopped, it returns the next hrtimer.
 *
 * Called from power state control code with interrupts disabled
 */
ktime_t tick_nohz_get_next_hrtimer(void)
{
	return __this_cpu_read(tick_cpu_device.evtdev)->next_event;
}

/**
 * tick_nohz_get_sleep_length - return the expected length of the current sleep
 * @delta_next: duration until the next event if the tick cannot be stopped
 *
 * Called from power state control code with interrupts disabled.
 *
 * The return value of this function and/or the value returned by it through the
 * @delta_next pointer can be negative which must be taken into account by its
 * callers.
 */
ktime_t tick_nohz_get_sleep_length(ktime_t *delta_next)
{
	struct clock_event_device *dev = __this_cpu_read(tick_cpu_device.evtdev);
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);
	int cpu = smp_processor_id();
	/*
	 * The idle entry time is expected to be a sufficient approximation of
	 * the current time at this point.
	 */
	ktime_t now = ts->idle_entrytime;
	ktime_t next_event;

	WARN_ON_ONCE(!ts->inidle);

	*delta_next = ktime_sub(dev->next_event, now);

	if (!can_stop_idle_tick(cpu, ts))
		return *delta_next;

	next_event = tick_nohz_next_event(ts, cpu);
	if (!next_event)
		return *delta_next;

	/*
	 * If the next highres timer to expire is earlier than next_event, the
	 * idle governor needs to know that.
	 */
	next_event = min_t(u64, next_event,
			   hrtimer_next_event_without(&ts->sched_timer));

	return ktime_sub(next_event, now);
}

/**
 * tick_nohz_get_idle_calls_cpu - return the current idle calls counter value
 * for a particular CPU.
 *
 * Called from the schedutil frequency scaling governor in scheduler context.
 */
unsigned long tick_nohz_get_idle_calls_cpu(int cpu)
{
	struct tick_sched *ts = tick_get_tick_sched(cpu);

	return ts->idle_calls;
}

/**
 * tick_nohz_get_idle_calls - return the current idle calls counter value
 *
 * Called from the schedutil frequency scaling governor in scheduler context.
 */
unsigned long tick_nohz_get_idle_calls(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);

	return ts->idle_calls;
}

static void tick_nohz_account_idle_ticks(struct tick_sched *ts)
{
#ifndef CONFIG_VIRT_CPU_ACCOUNTING_NATIVE
	unsigned long ticks;

	if (vtime_accounting_enabled_this_cpu())
		return;
	/*
	 * We stopped the tick in idle. Update process times would miss the
	 * time we slept as update_process_times does only a 1 tick
	 * accounting. Enforce that this is accounted to idle !
	 */
	ticks = jiffies - ts->idle_jiffies;
	/*
	 * We might be one off. Do not randomly account a huge number of ticks!
	 */
	if (ticks && ticks < LONG_MAX)
		account_idle_ticks(ticks);
#endif
}

static void __tick_nohz_idle_restart_tick(struct tick_sched *ts, ktime_t now)
{
	tick_nohz_restart_sched_tick(ts, now);
	tick_nohz_account_idle_ticks(ts);
}

void tick_nohz_idle_restart_tick(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);

	if (ts->tick_stopped)
		__tick_nohz_idle_restart_tick(ts, ktime_get());
}

/**
 * tick_nohz_idle_exit - restart the idle tick from the idle task
 *
 * Restart the idle tick when the CPU is woken up from idle
 * This also exit the RCU extended quiescent state. The CPU
 * can use RCU again after this function is called.
 */
void tick_nohz_idle_exit(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);
	bool idle_active, tick_stopped;
	ktime_t now;

	local_irq_disable();

	WARN_ON_ONCE(!ts->inidle);
	WARN_ON_ONCE(ts->timer_expires_base);

	ts->inidle = 0;
	idle_active = ts->idle_active;
	tick_stopped = ts->tick_stopped;

	if (idle_active || tick_stopped)
		now = ktime_get();

	if (idle_active)
		tick_nohz_stop_idle(ts, now);

	if (tick_stopped)
		__tick_nohz_idle_restart_tick(ts, now);

	local_irq_enable();
}

/*
 * The nohz low res interrupt handler
 */
static void tick_nohz_handler(struct clock_event_device *dev)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);
	struct pt_regs *regs = get_irq_regs();
	ktime_t now = ktime_get();

	dev->next_event = KTIME_MAX;

	tick_sched_do_timer(ts, now);
	tick_sched_handle(ts, regs);

	/* No need to reprogram if we are running tickless  */
	if (unlikely(ts->tick_stopped))
		return;

	hrtimer_forward(&ts->sched_timer, now, TICK_NSEC);
	tick_program_event(hrtimer_get_expires(&ts->sched_timer), 1);
}

static inline void tick_nohz_activate(struct tick_sched *ts, int mode)
{
	if (!tick_nohz_enabled)
		return;
	ts->nohz_mode = mode;
	/* One update is enough */
	if (!test_and_set_bit(0, &tick_nohz_active))
		timers_update_nohz();
}

/**
 * tick_nohz_switch_to_nohz - switch to nohz mode
 */
static void tick_nohz_switch_to_nohz(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);
	ktime_t next;

	if (!tick_nohz_enabled)
		return;

	if (tick_switch_to_oneshot(tick_nohz_handler))
		return;

	/*
	 * Recycle the hrtimer in ts, so we can share the
	 * hrtimer_forward with the highres code.
	 */
	hrtimer_init(&ts->sched_timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS_HARD);
	/* Get the next period */
	next = tick_init_jiffy_update();

	hrtimer_set_expires(&ts->sched_timer, next);
	hrtimer_forward_now(&ts->sched_timer, TICK_NSEC);
	tick_program_event(hrtimer_get_expires(&ts->sched_timer), 1);
	tick_nohz_activate(ts, NOHZ_MODE_LOWRES);
}

static inline void tick_nohz_irq_enter(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);
	ktime_t now;

	if (!ts->idle_active && !ts->tick_stopped)
		return;
	now = ktime_get();
	if (ts->idle_active)
		tick_nohz_stop_idle(ts, now);
	if (ts->tick_stopped)
		tick_nohz_update_jiffies(now);
}

#else

static inline void tick_nohz_switch_to_nohz(void) { }
static inline void tick_nohz_irq_enter(void) { }
static inline void tick_nohz_activate(struct tick_sched *ts, int mode) { }

#endif /* CONFIG_NO_HZ_COMMON */

/*
 * Called from irq_enter to notify about the possible interruption of idle()
 */
void tick_irq_enter(void)
{
	tick_check_oneshot_broadcast_this_cpu();
	tick_nohz_irq_enter();
}

/*
 * High resolution timer specific code
 */
#ifdef CONFIG_HIGH_RES_TIMERS
/*
 * We rearm the timer until we get disabled by the idle code.
 * Called with interrupts disabled.
 */
static enum hrtimer_restart tick_sched_timer(struct hrtimer *timer)
{
	struct tick_sched *ts =
		container_of(timer, struct tick_sched, sched_timer);
	struct pt_regs *regs = get_irq_regs();
	ktime_t now = ktime_get();

	tick_sched_do_timer(ts, now);

	/*
	 * Do not call, when we are not in irq context and have
	 * no valid regs pointer
	 */
	if (regs)
		tick_sched_handle(ts, regs);
	else
		ts->next_tick = 0;

	/* No need to reprogram if we are in idle or full dynticks mode */
	if (unlikely(ts->tick_stopped))
		return HRTIMER_NORESTART;

	hrtimer_forward(timer, now, TICK_NSEC);

	return HRTIMER_RESTART;
}

static int sched_skew_tick;

static int __init skew_tick(char *str)
{
	get_option(&str, &sched_skew_tick);

	return 0;
}
early_param("skew_tick", skew_tick);

/**
 * tick_setup_sched_timer - setup the tick emulation timer
 */
void tick_setup_sched_timer(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);
	ktime_t now = ktime_get();

	/*
	 * Emulate tick processing via per-CPU hrtimers:
	 */
	hrtimer_init(&ts->sched_timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS_HARD);
	ts->sched_timer.function = tick_sched_timer;

	/* Get the next period (per-CPU) */
	hrtimer_set_expires(&ts->sched_timer, tick_init_jiffy_update());

	/* Offset the tick to avert jiffies_lock contention. */
	if (sched_skew_tick) {
		u64 offset = TICK_NSEC >> 1;
		do_div(offset, num_possible_cpus());
		offset *= smp_processor_id();
		hrtimer_add_expires_ns(&ts->sched_timer, offset);
	}

	hrtimer_forward(&ts->sched_timer, now, TICK_NSEC);
	hrtimer_start_expires(&ts->sched_timer, HRTIMER_MODE_ABS_PINNED_HARD);
	tick_nohz_activate(ts, NOHZ_MODE_HIGHRES);
}
#endif /* HIGH_RES_TIMERS */

#if defined CONFIG_NO_HZ_COMMON || defined CONFIG_HIGH_RES_TIMERS
void tick_cancel_sched_timer(int cpu)
{
	struct tick_sched *ts = &per_cpu(tick_cpu_sched, cpu);

# ifdef CONFIG_HIGH_RES_TIMERS
	if (ts->sched_timer.base)
		hrtimer_cancel(&ts->sched_timer);
# endif

	memset(ts, 0, sizeof(*ts));
}
#endif

/**
 * Async notification about clocksource changes
 */
void tick_clock_notify(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		set_bit(0, &per_cpu(tick_cpu_sched, cpu).check_clocks);
}

/*
 * Async notification about clock event changes
 */
void tick_oneshot_notify(void)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);

	set_bit(0, &ts->check_clocks);
}

/**
 * Check, if a change happened, which makes oneshot possible.
 *
 * Called cyclic from the hrtimer softirq (driven by the timer
 * softirq) allow_nohz signals, that we can switch into low-res nohz
 * mode, because high resolution timers are disabled (either compile
 * or runtime). Called with interrupts disabled.
 */
int tick_check_oneshot_change(int allow_nohz)
{
	struct tick_sched *ts = this_cpu_ptr(&tick_cpu_sched);

	if (!test_and_clear_bit(0, &ts->check_clocks))
		return 0;

	if (ts->nohz_mode != NOHZ_MODE_INACTIVE)
		return 0;

	if (!timekeeping_valid_for_hres() || !tick_is_oneshot_available())
		return 0;

	if (!allow_nohz)
		return 1;

	tick_nohz_switch_to_nohz();
	return 0;
}
