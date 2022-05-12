/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef INTEL_RPS_H
#define INTEL_RPS_H

#include "intel_rps_types.h"

struct i915_request;

void intel_rps_init_early(struct intel_rps *rps);
void intel_rps_init(struct intel_rps *rps);
void intel_rps_sanitize(struct intel_rps *rps);

void intel_rps_driver_register(struct intel_rps *rps);
void intel_rps_driver_unregister(struct intel_rps *rps);

void intel_rps_enable(struct intel_rps *rps);
void intel_rps_disable(struct intel_rps *rps);

void intel_rps_park(struct intel_rps *rps);
void intel_rps_unpark(struct intel_rps *rps);
void intel_rps_boost(struct i915_request *rq);

int intel_rps_set(struct intel_rps *rps, u8 val);
void intel_rps_mark_interactive(struct intel_rps *rps, bool interactive);

int intel_gpu_freq(struct intel_rps *rps, int val);
int intel_freq_opcode(struct intel_rps *rps, int val);
u32 intel_rps_get_cagf(struct intel_rps *rps, u32 rpstat1);
u32 intel_rps_read_actual_frequency(struct intel_rps *rps);

void gen5_rps_irq_handler(struct intel_rps *rps);
void gen6_rps_irq_handler(struct intel_rps *rps, u32 pm_iir);
void gen11_rps_irq_handler(struct intel_rps *rps, u32 pm_iir);

static inline bool intel_rps_is_enabled(const struct intel_rps *rps)
{
	return test_bit(INTEL_RPS_ENABLED, &rps->flags);
}

static inline void intel_rps_set_enabled(struct intel_rps *rps)
{
	set_bit(INTEL_RPS_ENABLED, &rps->flags);
}

static inline void intel_rps_clear_enabled(struct intel_rps *rps)
{
	clear_bit(INTEL_RPS_ENABLED, &rps->flags);
}

static inline bool intel_rps_is_active(const struct intel_rps *rps)
{
	return test_bit(INTEL_RPS_ACTIVE, &rps->flags);
}

static inline void intel_rps_set_active(struct intel_rps *rps)
{
	set_bit(INTEL_RPS_ACTIVE, &rps->flags);
}

static inline bool intel_rps_clear_active(struct intel_rps *rps)
{
	return test_and_clear_bit(INTEL_RPS_ACTIVE, &rps->flags);
}

static inline bool intel_rps_has_interrupts(const struct intel_rps *rps)
{
	return test_bit(INTEL_RPS_INTERRUPTS, &rps->flags);
}

static inline void intel_rps_set_interrupts(struct intel_rps *rps)
{
	set_bit(INTEL_RPS_INTERRUPTS, &rps->flags);
}

static inline void intel_rps_clear_interrupts(struct intel_rps *rps)
{
	clear_bit(INTEL_RPS_INTERRUPTS, &rps->flags);
}

static inline bool intel_rps_uses_timer(const struct intel_rps *rps)
{
	return test_bit(INTEL_RPS_TIMER, &rps->flags);
}

static inline void intel_rps_set_timer(struct intel_rps *rps)
{
	set_bit(INTEL_RPS_TIMER, &rps->flags);
}

static inline void intel_rps_clear_timer(struct intel_rps *rps)
{
	clear_bit(INTEL_RPS_TIMER, &rps->flags);
}

#endif /* INTEL_RPS_H */
