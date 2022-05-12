/*
 * ladder.c - the residency ladder algorithm
 *
 *  Copyright (C) 2001, 2002 Andy Grover <andrew.grover@intel.com>
 *  Copyright (C) 2001, 2002 Paul Diefenbaugh <paul.s.diefenbaugh@intel.com>
 *  Copyright (C) 2004, 2005 Dominik Brodowski <linux@brodo.de>
 *
 * (C) 2006-2007 Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>
 *               Shaohua Li <shaohua.li@intel.com>
 *               Adam Belay <abelay@novell.com>
 *
 * This code is licenced under the GPL.
 */

#include <linux/kernel.h>
#include <linux/cpuidle.h>
#include <linux/jiffies.h>
#include <linux/tick.h>

#include <asm/io.h>
#include <linux/uaccess.h>

#define PROMOTION_COUNT 4
#define DEMOTION_COUNT 1

struct ladder_device_state {
	struct {
		u32 promotion_count;
		u32 demotion_count;
		u64 promotion_time_ns;
		u64 demotion_time_ns;
	} threshold;
	struct {
		int promotion_count;
		int demotion_count;
	} stats;
};

struct ladder_device {
	struct ladder_device_state states[CPUIDLE_STATE_MAX];
};

static DEFINE_PER_CPU(struct ladder_device, ladder_devices);

/**
 * ladder_do_selection - prepares private data for a state change
 * @ldev: the ladder device
 * @old_idx: the current state index
 * @new_idx: the new target state index
 */
static inline void ladder_do_selection(struct cpuidle_device *dev,
				       struct ladder_device *ldev,
				       int old_idx, int new_idx)
{
	ldev->states[old_idx].stats.promotion_count = 0;
	ldev->states[old_idx].stats.demotion_count = 0;
	dev->last_state_idx = new_idx;
}

/**
 * ladder_select_state - selects the next state to enter
 * @drv: cpuidle driver
 * @dev: the CPU
 * @dummy: not used
 */
static int ladder_select_state(struct cpuidle_driver *drv,
			       struct cpuidle_device *dev, bool *dummy)
{
	struct ladder_device *ldev = this_cpu_ptr(&ladder_devices);
	struct ladder_device_state *last_state;
	int last_idx = dev->last_state_idx;
	int first_idx = drv->states[0].flags & CPUIDLE_FLAG_POLLING ? 1 : 0;
	s64 latency_req = cpuidle_governor_latency_req(dev->cpu);
	s64 last_residency;

	/* Special case when user has set very strict latency requirement */
	if (unlikely(latency_req == 0)) {
		ladder_do_selection(dev, ldev, last_idx, 0);
		return 0;
	}

	last_state = &ldev->states[last_idx];

	last_residency = dev->last_residency_ns - drv->states[last_idx].exit_latency_ns;

	/* consider promotion */
	if (last_idx < drv->state_count - 1 &&
	    !dev->states_usage[last_idx + 1].disable &&
	    last_residency > last_state->threshold.promotion_time_ns &&
	    drv->states[last_idx + 1].exit_latency_ns <= latency_req) {
		last_state->stats.promotion_count++;
		last_state->stats.demotion_count = 0;
		if (last_state->stats.promotion_count >= last_state->threshold.promotion_count) {
			ladder_do_selection(dev, ldev, last_idx, last_idx + 1);
			return last_idx + 1;
		}
	}

	/* consider demotion */
	if (last_idx > first_idx &&
	    (dev->states_usage[last_idx].disable ||
	    drv->states[last_idx].exit_latency_ns > latency_req)) {
		int i;

		for (i = last_idx - 1; i > first_idx; i--) {
			if (drv->states[i].exit_latency_ns <= latency_req)
				break;
		}
		ladder_do_selection(dev, ldev, last_idx, i);
		return i;
	}

	if (last_idx > first_idx &&
	    last_residency < last_state->threshold.demotion_time_ns) {
		last_state->stats.demotion_count++;
		last_state->stats.promotion_count = 0;
		if (last_state->stats.demotion_count >= last_state->threshold.demotion_count) {
			ladder_do_selection(dev, ldev, last_idx, last_idx - 1);
			return last_idx - 1;
		}
	}

	/* otherwise remain at the current state */
	return last_idx;
}

/**
 * ladder_enable_device - setup for the governor
 * @drv: cpuidle driver
 * @dev: the CPU
 */
static int ladder_enable_device(struct cpuidle_driver *drv,
				struct cpuidle_device *dev)
{
	int i;
	int first_idx = drv->states[0].flags & CPUIDLE_FLAG_POLLING ? 1 : 0;
	struct ladder_device *ldev = &per_cpu(ladder_devices, dev->cpu);
	struct ladder_device_state *lstate;
	struct cpuidle_state *state;

	dev->last_state_idx = first_idx;

	for (i = first_idx; i < drv->state_count; i++) {
		state = &drv->states[i];
		lstate = &ldev->states[i];

		lstate->stats.promotion_count = 0;
		lstate->stats.demotion_count = 0;

		lstate->threshold.promotion_count = PROMOTION_COUNT;
		lstate->threshold.demotion_count = DEMOTION_COUNT;

		if (i < drv->state_count - 1)
			lstate->threshold.promotion_time_ns = state->exit_latency_ns;
		if (i > first_idx)
			lstate->threshold.demotion_time_ns = state->exit_latency_ns;
	}

	return 0;
}

/**
 * ladder_reflect - update the correct last_state_idx
 * @dev: the CPU
 * @index: the index of actual state entered
 */
static void ladder_reflect(struct cpuidle_device *dev, int index)
{
	if (index > 0)
		dev->last_state_idx = index;
}

static struct cpuidle_governor ladder_governor = {
	.name =		"ladder",
	.rating =	10,
	.enable =	ladder_enable_device,
	.select =	ladder_select_state,
	.reflect =	ladder_reflect,
};

/**
 * init_ladder - initializes the governor
 */
static int __init init_ladder(void)
{
	/*
	 * When NO_HZ is disabled, or when booting with nohz=off, the ladder
	 * governor is better so give it a higher rating than the menu
	 * governor.
	 */
	if (!tick_nohz_enabled)
		ladder_governor.rating = 25;

	return cpuidle_register_governor(&ladder_governor);
}

postcore_initcall(init_ladder);
