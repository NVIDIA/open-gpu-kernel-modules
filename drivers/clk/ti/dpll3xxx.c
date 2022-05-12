// SPDX-License-Identifier: GPL-2.0-only
/*
 * OMAP3/4 - specific DPLL control functions
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Copyright (C) 2009-2010 Nokia Corporation
 *
 * Written by Paul Walmsley
 * Testing and integration fixes by Jouni Högander
 *
 * 36xx support added by Vishwanath BS, Richard Woodruff, and Nishanth
 * Menon
 *
 * Parts of this code are based on code written by
 * Richard Woodruff, Tony Lindgren, Tuukka Tikkanen, Karthik Dasu
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/clkdev.h>
#include <linux/clk/ti.h>

#include "clock.h"

/* CM_AUTOIDLE_PLL*.AUTO_* bit values */
#define DPLL_AUTOIDLE_DISABLE			0x0
#define DPLL_AUTOIDLE_LOW_POWER_STOP		0x1

#define MAX_DPLL_WAIT_TRIES		1000000

#define OMAP3XXX_EN_DPLL_LOCKED		0x7

/* Forward declarations */
static u32 omap3_dpll_autoidle_read(struct clk_hw_omap *clk);
static void omap3_dpll_deny_idle(struct clk_hw_omap *clk);
static void omap3_dpll_allow_idle(struct clk_hw_omap *clk);

/* Private functions */

/* _omap3_dpll_write_clken - write clken_bits arg to a DPLL's enable bits */
static void _omap3_dpll_write_clken(struct clk_hw_omap *clk, u8 clken_bits)
{
	const struct dpll_data *dd;
	u32 v;

	dd = clk->dpll_data;

	v = ti_clk_ll_ops->clk_readl(&dd->control_reg);
	v &= ~dd->enable_mask;
	v |= clken_bits << __ffs(dd->enable_mask);
	ti_clk_ll_ops->clk_writel(v, &dd->control_reg);
}

/* _omap3_wait_dpll_status: wait for a DPLL to enter a specific state */
static int _omap3_wait_dpll_status(struct clk_hw_omap *clk, u8 state)
{
	const struct dpll_data *dd;
	int i = 0;
	int ret = -EINVAL;
	const char *clk_name;

	dd = clk->dpll_data;
	clk_name = clk_hw_get_name(&clk->hw);

	state <<= __ffs(dd->idlest_mask);

	while (((ti_clk_ll_ops->clk_readl(&dd->idlest_reg) & dd->idlest_mask)
		!= state) && i < MAX_DPLL_WAIT_TRIES) {
		i++;
		udelay(1);
	}

	if (i == MAX_DPLL_WAIT_TRIES) {
		pr_err("clock: %s failed transition to '%s'\n",
		       clk_name, (state) ? "locked" : "bypassed");
	} else {
		pr_debug("clock: %s transition to '%s' in %d loops\n",
			 clk_name, (state) ? "locked" : "bypassed", i);

		ret = 0;
	}

	return ret;
}

/* From 3430 TRM ES2 4.7.6.2 */
static u16 _omap3_dpll_compute_freqsel(struct clk_hw_omap *clk, u8 n)
{
	unsigned long fint;
	u16 f = 0;

	fint = clk_hw_get_rate(clk->dpll_data->clk_ref) / n;

	pr_debug("clock: fint is %lu\n", fint);

	if (fint >= 750000 && fint <= 1000000)
		f = 0x3;
	else if (fint > 1000000 && fint <= 1250000)
		f = 0x4;
	else if (fint > 1250000 && fint <= 1500000)
		f = 0x5;
	else if (fint > 1500000 && fint <= 1750000)
		f = 0x6;
	else if (fint > 1750000 && fint <= 2100000)
		f = 0x7;
	else if (fint > 7500000 && fint <= 10000000)
		f = 0xB;
	else if (fint > 10000000 && fint <= 12500000)
		f = 0xC;
	else if (fint > 12500000 && fint <= 15000000)
		f = 0xD;
	else if (fint > 15000000 && fint <= 17500000)
		f = 0xE;
	else if (fint > 17500000 && fint <= 21000000)
		f = 0xF;
	else
		pr_debug("clock: unknown freqsel setting for %d\n", n);

	return f;
}

/**
 * _omap3_noncore_dpll_lock - instruct a DPLL to lock and wait for readiness
 * @clk: pointer to a DPLL struct clk
 *
 * Instructs a non-CORE DPLL to lock.  Waits for the DPLL to report
 * readiness before returning.  Will save and restore the DPLL's
 * autoidle state across the enable, per the CDP code.  If the DPLL
 * locked successfully, return 0; if the DPLL did not lock in the time
 * allotted, or DPLL3 was passed in, return -EINVAL.
 */
static int _omap3_noncore_dpll_lock(struct clk_hw_omap *clk)
{
	const struct dpll_data *dd;
	u8 ai;
	u8 state = 1;
	int r = 0;

	pr_debug("clock: locking DPLL %s\n", clk_hw_get_name(&clk->hw));

	dd = clk->dpll_data;
	state <<= __ffs(dd->idlest_mask);

	/* Check if already locked */
	if ((ti_clk_ll_ops->clk_readl(&dd->idlest_reg) & dd->idlest_mask) ==
	    state)
		goto done;

	ai = omap3_dpll_autoidle_read(clk);

	if (ai)
		omap3_dpll_deny_idle(clk);

	_omap3_dpll_write_clken(clk, DPLL_LOCKED);

	r = _omap3_wait_dpll_status(clk, 1);

	if (ai)
		omap3_dpll_allow_idle(clk);

done:
	return r;
}

/**
 * _omap3_noncore_dpll_bypass - instruct a DPLL to bypass and wait for readiness
 * @clk: pointer to a DPLL struct clk
 *
 * Instructs a non-CORE DPLL to enter low-power bypass mode.  In
 * bypass mode, the DPLL's rate is set equal to its parent clock's
 * rate.  Waits for the DPLL to report readiness before returning.
 * Will save and restore the DPLL's autoidle state across the enable,
 * per the CDP code.  If the DPLL entered bypass mode successfully,
 * return 0; if the DPLL did not enter bypass in the time allotted, or
 * DPLL3 was passed in, or the DPLL does not support low-power bypass,
 * return -EINVAL.
 */
static int _omap3_noncore_dpll_bypass(struct clk_hw_omap *clk)
{
	int r;
	u8 ai;

	if (!(clk->dpll_data->modes & (1 << DPLL_LOW_POWER_BYPASS)))
		return -EINVAL;

	pr_debug("clock: configuring DPLL %s for low-power bypass\n",
		 clk_hw_get_name(&clk->hw));

	ai = omap3_dpll_autoidle_read(clk);

	_omap3_dpll_write_clken(clk, DPLL_LOW_POWER_BYPASS);

	r = _omap3_wait_dpll_status(clk, 0);

	if (ai)
		omap3_dpll_allow_idle(clk);

	return r;
}

/**
 * _omap3_noncore_dpll_stop - instruct a DPLL to stop
 * @clk: pointer to a DPLL struct clk
 *
 * Instructs a non-CORE DPLL to enter low-power stop. Will save and
 * restore the DPLL's autoidle state across the stop, per the CDP
 * code.  If DPLL3 was passed in, or the DPLL does not support
 * low-power stop, return -EINVAL; otherwise, return 0.
 */
static int _omap3_noncore_dpll_stop(struct clk_hw_omap *clk)
{
	u8 ai;

	if (!(clk->dpll_data->modes & (1 << DPLL_LOW_POWER_STOP)))
		return -EINVAL;

	pr_debug("clock: stopping DPLL %s\n", clk_hw_get_name(&clk->hw));

	ai = omap3_dpll_autoidle_read(clk);

	_omap3_dpll_write_clken(clk, DPLL_LOW_POWER_STOP);

	if (ai)
		omap3_dpll_allow_idle(clk);

	return 0;
}

/**
 * _lookup_dco - Lookup DCO used by j-type DPLL
 * @clk: pointer to a DPLL struct clk
 * @dco: digital control oscillator selector
 * @m: DPLL multiplier to set
 * @n: DPLL divider to set
 *
 * See 36xx TRM section 3.5.3.3.3.2 "Type B DPLL (Low-Jitter)"
 *
 * XXX This code is not needed for 3430/AM35xx; can it be optimized
 * out in non-multi-OMAP builds for those chips?
 */
static void _lookup_dco(struct clk_hw_omap *clk, u8 *dco, u16 m, u8 n)
{
	unsigned long fint, clkinp; /* watch out for overflow */

	clkinp = clk_hw_get_rate(clk_hw_get_parent(&clk->hw));
	fint = (clkinp / n) * m;

	if (fint < 1000000000)
		*dco = 2;
	else
		*dco = 4;
}

/**
 * _lookup_sddiv - Calculate sigma delta divider for j-type DPLL
 * @clk: pointer to a DPLL struct clk
 * @sd_div: target sigma-delta divider
 * @m: DPLL multiplier to set
 * @n: DPLL divider to set
 *
 * See 36xx TRM section 3.5.3.3.3.2 "Type B DPLL (Low-Jitter)"
 *
 * XXX This code is not needed for 3430/AM35xx; can it be optimized
 * out in non-multi-OMAP builds for those chips?
 */
static void _lookup_sddiv(struct clk_hw_omap *clk, u8 *sd_div, u16 m, u8 n)
{
	unsigned long clkinp, sd; /* watch out for overflow */
	int mod1, mod2;

	clkinp = clk_hw_get_rate(clk_hw_get_parent(&clk->hw));

	/*
	 * target sigma-delta to near 250MHz
	 * sd = ceil[(m/(n+1)) * (clkinp_MHz / 250)]
	 */
	clkinp /= 100000; /* shift from MHz to 10*Hz for 38.4 and 19.2 */
	mod1 = (clkinp * m) % (250 * n);
	sd = (clkinp * m) / (250 * n);
	mod2 = sd % 10;
	sd /= 10;

	if (mod1 || mod2)
		sd++;
	*sd_div = sd;
}

/**
 * _omap3_noncore_dpll_program - set non-core DPLL M,N values directly
 * @clk:	struct clk * of DPLL to set
 * @freqsel:	FREQSEL value to set
 *
 * Program the DPLL with the last M, N values calculated, and wait for
 * the DPLL to lock. Returns -EINVAL upon error, or 0 upon success.
 */
static int omap3_noncore_dpll_program(struct clk_hw_omap *clk, u16 freqsel)
{
	struct dpll_data *dd = clk->dpll_data;
	u8 dco, sd_div, ai = 0;
	u32 v;
	bool errata_i810;

	/* 3430 ES2 TRM: 4.7.6.9 DPLL Programming Sequence */
	_omap3_noncore_dpll_bypass(clk);

	/*
	 * Set jitter correction. Jitter correction applicable for OMAP343X
	 * only since freqsel field is no longer present on other devices.
	 */
	if (ti_clk_get_features()->flags & TI_CLK_DPLL_HAS_FREQSEL) {
		v = ti_clk_ll_ops->clk_readl(&dd->control_reg);
		v &= ~dd->freqsel_mask;
		v |= freqsel << __ffs(dd->freqsel_mask);
		ti_clk_ll_ops->clk_writel(v, &dd->control_reg);
	}

	/* Set DPLL multiplier, divider */
	v = ti_clk_ll_ops->clk_readl(&dd->mult_div1_reg);

	/* Handle Duty Cycle Correction */
	if (dd->dcc_mask) {
		if (dd->last_rounded_rate >= dd->dcc_rate)
			v |= dd->dcc_mask; /* Enable DCC */
		else
			v &= ~dd->dcc_mask; /* Disable DCC */
	}

	v &= ~(dd->mult_mask | dd->div1_mask);
	v |= dd->last_rounded_m << __ffs(dd->mult_mask);
	v |= (dd->last_rounded_n - 1) << __ffs(dd->div1_mask);

	/* Configure dco and sd_div for dplls that have these fields */
	if (dd->dco_mask) {
		_lookup_dco(clk, &dco, dd->last_rounded_m, dd->last_rounded_n);
		v &= ~(dd->dco_mask);
		v |= dco << __ffs(dd->dco_mask);
	}
	if (dd->sddiv_mask) {
		_lookup_sddiv(clk, &sd_div, dd->last_rounded_m,
			      dd->last_rounded_n);
		v &= ~(dd->sddiv_mask);
		v |= sd_div << __ffs(dd->sddiv_mask);
	}

	/*
	 * Errata i810 - DPLL controller can get stuck while transitioning
	 * to a power saving state. Software must ensure the DPLL can not
	 * transition to a low power state while changing M/N values.
	 * Easiest way to accomplish this is to prevent DPLL autoidle
	 * before doing the M/N re-program.
	 */
	errata_i810 = ti_clk_get_features()->flags & TI_CLK_ERRATA_I810;

	if (errata_i810) {
		ai = omap3_dpll_autoidle_read(clk);
		if (ai) {
			omap3_dpll_deny_idle(clk);

			/* OCP barrier */
			omap3_dpll_autoidle_read(clk);
		}
	}

	ti_clk_ll_ops->clk_writel(v, &dd->mult_div1_reg);

	/* Set 4X multiplier and low-power mode */
	if (dd->m4xen_mask || dd->lpmode_mask) {
		v = ti_clk_ll_ops->clk_readl(&dd->control_reg);

		if (dd->m4xen_mask) {
			if (dd->last_rounded_m4xen)
				v |= dd->m4xen_mask;
			else
				v &= ~dd->m4xen_mask;
		}

		if (dd->lpmode_mask) {
			if (dd->last_rounded_lpmode)
				v |= dd->lpmode_mask;
			else
				v &= ~dd->lpmode_mask;
		}

		ti_clk_ll_ops->clk_writel(v, &dd->control_reg);
	}

	/* We let the clock framework set the other output dividers later */

	/* REVISIT: Set ramp-up delay? */

	_omap3_noncore_dpll_lock(clk);

	if (errata_i810 && ai)
		omap3_dpll_allow_idle(clk);

	return 0;
}

/* Public functions */

/**
 * omap3_dpll_recalc - recalculate DPLL rate
 * @hw: struct clk_hw containing the DPLL struct clk
 * @parent_rate: clock rate of the DPLL parent
 *
 * Recalculate and propagate the DPLL rate.
 */
unsigned long omap3_dpll_recalc(struct clk_hw *hw, unsigned long parent_rate)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);

	return omap2_get_dpll_rate(clk);
}

/* Non-CORE DPLL (e.g., DPLLs that do not control SDRC) clock functions */

/**
 * omap3_noncore_dpll_enable - instruct a DPLL to enter bypass or lock mode
 * @hw: struct clk_hw containing then pointer to a DPLL struct clk
 *
 * Instructs a non-CORE DPLL to enable, e.g., to enter bypass or lock.
 * The choice of modes depends on the DPLL's programmed rate: if it is
 * the same as the DPLL's parent clock, it will enter bypass;
 * otherwise, it will enter lock.  This code will wait for the DPLL to
 * indicate readiness before returning, unless the DPLL takes too long
 * to enter the target state.  Intended to be used as the struct clk's
 * enable function.  If DPLL3 was passed in, or the DPLL does not
 * support low-power stop, or if the DPLL took too long to enter
 * bypass or lock, return -EINVAL; otherwise, return 0.
 */
int omap3_noncore_dpll_enable(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	int r;
	struct dpll_data *dd;
	struct clk_hw *parent;

	dd = clk->dpll_data;
	if (!dd)
		return -EINVAL;

	if (clk->clkdm) {
		r = ti_clk_ll_ops->clkdm_clk_enable(clk->clkdm, hw->clk);
		if (r) {
			WARN(1,
			     "%s: could not enable %s's clockdomain %s: %d\n",
			     __func__, clk_hw_get_name(hw),
			     clk->clkdm_name, r);
			return r;
		}
	}

	parent = clk_hw_get_parent(hw);

	if (clk_hw_get_rate(hw) == clk_hw_get_rate(dd->clk_bypass)) {
		WARN_ON(parent != dd->clk_bypass);
		r = _omap3_noncore_dpll_bypass(clk);
	} else {
		WARN_ON(parent != dd->clk_ref);
		r = _omap3_noncore_dpll_lock(clk);
	}

	return r;
}

/**
 * omap3_noncore_dpll_disable - instruct a DPLL to enter low-power stop
 * @hw: struct clk_hw containing then pointer to a DPLL struct clk
 *
 * Instructs a non-CORE DPLL to enter low-power stop.  This function is
 * intended for use in struct clkops.  No return value.
 */
void omap3_noncore_dpll_disable(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);

	_omap3_noncore_dpll_stop(clk);
	if (clk->clkdm)
		ti_clk_ll_ops->clkdm_clk_disable(clk->clkdm, hw->clk);
}

/* Non-CORE DPLL rate set code */

/**
 * omap3_noncore_dpll_determine_rate - determine rate for a DPLL
 * @hw: pointer to the clock to determine rate for
 * @req: target rate request
 *
 * Determines which DPLL mode to use for reaching a desired target rate.
 * Checks whether the DPLL shall be in bypass or locked mode, and if
 * locked, calculates the M,N values for the DPLL via round-rate.
 * Returns a 0 on success, negative error value in failure.
 */
int omap3_noncore_dpll_determine_rate(struct clk_hw *hw,
				      struct clk_rate_request *req)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	struct dpll_data *dd;

	if (!req->rate)
		return -EINVAL;

	dd = clk->dpll_data;
	if (!dd)
		return -EINVAL;

	if (clk_hw_get_rate(dd->clk_bypass) == req->rate &&
	    (dd->modes & (1 << DPLL_LOW_POWER_BYPASS))) {
		req->best_parent_hw = dd->clk_bypass;
	} else {
		req->rate = omap2_dpll_round_rate(hw, req->rate,
					  &req->best_parent_rate);
		req->best_parent_hw = dd->clk_ref;
	}

	req->best_parent_rate = req->rate;

	return 0;
}

/**
 * omap3_noncore_dpll_set_parent - set parent for a DPLL clock
 * @hw: pointer to the clock to set parent for
 * @index: parent index to select
 *
 * Sets parent for a DPLL clock. This sets the DPLL into bypass or
 * locked mode. Returns 0 with success, negative error value otherwise.
 */
int omap3_noncore_dpll_set_parent(struct clk_hw *hw, u8 index)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	int ret;

	if (!hw)
		return -EINVAL;

	if (index)
		ret = _omap3_noncore_dpll_bypass(clk);
	else
		ret = _omap3_noncore_dpll_lock(clk);

	return ret;
}

/**
 * omap3_noncore_dpll_set_rate - set rate for a DPLL clock
 * @hw: pointer to the clock to set parent for
 * @rate: target rate for the clock
 * @parent_rate: rate of the parent clock
 *
 * Sets rate for a DPLL clock. First checks if the clock parent is
 * reference clock (in bypass mode, the rate of the clock can't be
 * changed) and proceeds with the rate change operation. Returns 0
 * with success, negative error value otherwise.
 */
int omap3_noncore_dpll_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	struct dpll_data *dd;
	u16 freqsel = 0;
	int ret;

	if (!hw || !rate)
		return -EINVAL;

	dd = clk->dpll_data;
	if (!dd)
		return -EINVAL;

	if (clk_hw_get_parent(hw) != dd->clk_ref)
		return -EINVAL;

	if (dd->last_rounded_rate == 0)
		return -EINVAL;

	/* Freqsel is available only on OMAP343X devices */
	if (ti_clk_get_features()->flags & TI_CLK_DPLL_HAS_FREQSEL) {
		freqsel = _omap3_dpll_compute_freqsel(clk, dd->last_rounded_n);
		WARN_ON(!freqsel);
	}

	pr_debug("%s: %s: set rate: locking rate to %lu.\n", __func__,
		 clk_hw_get_name(hw), rate);

	ret = omap3_noncore_dpll_program(clk, freqsel);

	return ret;
}

/**
 * omap3_noncore_dpll_set_rate_and_parent - set rate and parent for a DPLL clock
 * @hw: pointer to the clock to set rate and parent for
 * @rate: target rate for the DPLL
 * @parent_rate: clock rate of the DPLL parent
 * @index: new parent index for the DPLL, 0 - reference, 1 - bypass
 *
 * Sets rate and parent for a DPLL clock. If new parent is the bypass
 * clock, only selects the parent. Otherwise proceeds with a rate
 * change, as this will effectively also change the parent as the
 * DPLL is put into locked mode. Returns 0 with success, negative error
 * value otherwise.
 */
int omap3_noncore_dpll_set_rate_and_parent(struct clk_hw *hw,
					   unsigned long rate,
					   unsigned long parent_rate,
					   u8 index)
{
	int ret;

	if (!hw || !rate)
		return -EINVAL;

	/*
	 * clk-ref at index[0], in which case we only need to set rate,
	 * the parent will be changed automatically with the lock sequence.
	 * With clk-bypass case we only need to change parent.
	 */
	if (index)
		ret = omap3_noncore_dpll_set_parent(hw, index);
	else
		ret = omap3_noncore_dpll_set_rate(hw, rate, parent_rate);

	return ret;
}

/* DPLL autoidle read/set code */

/**
 * omap3_dpll_autoidle_read - read a DPLL's autoidle bits
 * @clk: struct clk * of the DPLL to read
 *
 * Return the DPLL's autoidle bits, shifted down to bit 0.  Returns
 * -EINVAL if passed a null pointer or if the struct clk does not
 * appear to refer to a DPLL.
 */
static u32 omap3_dpll_autoidle_read(struct clk_hw_omap *clk)
{
	const struct dpll_data *dd;
	u32 v;

	if (!clk || !clk->dpll_data)
		return -EINVAL;

	dd = clk->dpll_data;

	if (!dd->autoidle_mask)
		return -EINVAL;

	v = ti_clk_ll_ops->clk_readl(&dd->autoidle_reg);
	v &= dd->autoidle_mask;
	v >>= __ffs(dd->autoidle_mask);

	return v;
}

/**
 * omap3_dpll_allow_idle - enable DPLL autoidle bits
 * @clk: struct clk * of the DPLL to operate on
 *
 * Enable DPLL automatic idle control.  This automatic idle mode
 * switching takes effect only when the DPLL is locked, at least on
 * OMAP3430.  The DPLL will enter low-power stop when its downstream
 * clocks are gated.  No return value.
 */
static void omap3_dpll_allow_idle(struct clk_hw_omap *clk)
{
	const struct dpll_data *dd;
	u32 v;

	if (!clk || !clk->dpll_data)
		return;

	dd = clk->dpll_data;

	if (!dd->autoidle_mask)
		return;

	/*
	 * REVISIT: CORE DPLL can optionally enter low-power bypass
	 * by writing 0x5 instead of 0x1.  Add some mechanism to
	 * optionally enter this mode.
	 */
	v = ti_clk_ll_ops->clk_readl(&dd->autoidle_reg);
	v &= ~dd->autoidle_mask;
	v |= DPLL_AUTOIDLE_LOW_POWER_STOP << __ffs(dd->autoidle_mask);
	ti_clk_ll_ops->clk_writel(v, &dd->autoidle_reg);
}

/**
 * omap3_dpll_deny_idle - prevent DPLL from automatically idling
 * @clk: struct clk * of the DPLL to operate on
 *
 * Disable DPLL automatic idle control.  No return value.
 */
static void omap3_dpll_deny_idle(struct clk_hw_omap *clk)
{
	const struct dpll_data *dd;
	u32 v;

	if (!clk || !clk->dpll_data)
		return;

	dd = clk->dpll_data;

	if (!dd->autoidle_mask)
		return;

	v = ti_clk_ll_ops->clk_readl(&dd->autoidle_reg);
	v &= ~dd->autoidle_mask;
	v |= DPLL_AUTOIDLE_DISABLE << __ffs(dd->autoidle_mask);
	ti_clk_ll_ops->clk_writel(v, &dd->autoidle_reg);
}

/* Clock control for DPLL outputs */

/* Find the parent DPLL for the given clkoutx2 clock */
static struct clk_hw_omap *omap3_find_clkoutx2_dpll(struct clk_hw *hw)
{
	struct clk_hw_omap *pclk = NULL;

	/* Walk up the parents of clk, looking for a DPLL */
	do {
		do {
			hw = clk_hw_get_parent(hw);
		} while (hw && (!omap2_clk_is_hw_omap(hw)));
		if (!hw)
			break;
		pclk = to_clk_hw_omap(hw);
	} while (pclk && !pclk->dpll_data);

	/* clk does not have a DPLL as a parent?  error in the clock data */
	if (!pclk) {
		WARN_ON(1);
		return NULL;
	}

	return pclk;
}

/**
 * omap3_clkoutx2_recalc - recalculate DPLL X2 output virtual clock rate
 * @hw: pointer  struct clk_hw
 * @parent_rate: clock rate of the DPLL parent
 *
 * Using parent clock DPLL data, look up DPLL state.  If locked, set our
 * rate to the dpll_clk * 2; otherwise, just use dpll_clk.
 */
unsigned long omap3_clkoutx2_recalc(struct clk_hw *hw,
				    unsigned long parent_rate)
{
	const struct dpll_data *dd;
	unsigned long rate;
	u32 v;
	struct clk_hw_omap *pclk = NULL;

	if (!parent_rate)
		return 0;

	pclk = omap3_find_clkoutx2_dpll(hw);

	if (!pclk)
		return 0;

	dd = pclk->dpll_data;

	WARN_ON(!dd->enable_mask);

	v = ti_clk_ll_ops->clk_readl(&dd->control_reg) & dd->enable_mask;
	v >>= __ffs(dd->enable_mask);
	if ((v != OMAP3XXX_EN_DPLL_LOCKED) || (dd->flags & DPLL_J_TYPE))
		rate = parent_rate;
	else
		rate = parent_rate * 2;
	return rate;
}

/**
 * omap3_core_dpll_save_context - Save the m and n values of the divider
 * @hw: pointer  struct clk_hw
 *
 * Before the dpll registers are lost save the last rounded rate m and n
 * and the enable mask.
 */
int omap3_core_dpll_save_context(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	struct dpll_data *dd;
	u32 v;

	dd = clk->dpll_data;

	v = ti_clk_ll_ops->clk_readl(&dd->control_reg);
	clk->context = (v & dd->enable_mask) >> __ffs(dd->enable_mask);

	if (clk->context == DPLL_LOCKED) {
		v = ti_clk_ll_ops->clk_readl(&dd->mult_div1_reg);
		dd->last_rounded_m = (v & dd->mult_mask) >>
						__ffs(dd->mult_mask);
		dd->last_rounded_n = ((v & dd->div1_mask) >>
						__ffs(dd->div1_mask)) + 1;
	}

	return 0;
}

/**
 * omap3_core_dpll_restore_context - restore the m and n values of the divider
 * @hw: pointer  struct clk_hw
 *
 * Restore the last rounded rate m and n
 * and the enable mask.
 */
void omap3_core_dpll_restore_context(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	const struct dpll_data *dd;
	u32 v;

	dd = clk->dpll_data;

	if (clk->context == DPLL_LOCKED) {
		_omap3_dpll_write_clken(clk, 0x4);
		_omap3_wait_dpll_status(clk, 0);

		v = ti_clk_ll_ops->clk_readl(&dd->mult_div1_reg);
		v &= ~(dd->mult_mask | dd->div1_mask);
		v |= dd->last_rounded_m << __ffs(dd->mult_mask);
		v |= (dd->last_rounded_n - 1) << __ffs(dd->div1_mask);
		ti_clk_ll_ops->clk_writel(v, &dd->mult_div1_reg);

		_omap3_dpll_write_clken(clk, DPLL_LOCKED);
		_omap3_wait_dpll_status(clk, 1);
	} else {
		_omap3_dpll_write_clken(clk, clk->context);
	}
}

/**
 * omap3_non_core_dpll_save_context - Save the m and n values of the divider
 * @hw: pointer  struct clk_hw
 *
 * Before the dpll registers are lost save the last rounded rate m and n
 * and the enable mask.
 */
int omap3_noncore_dpll_save_context(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	struct dpll_data *dd;
	u32 v;

	dd = clk->dpll_data;

	v = ti_clk_ll_ops->clk_readl(&dd->control_reg);
	clk->context = (v & dd->enable_mask) >> __ffs(dd->enable_mask);

	if (clk->context == DPLL_LOCKED) {
		v = ti_clk_ll_ops->clk_readl(&dd->mult_div1_reg);
		dd->last_rounded_m = (v & dd->mult_mask) >>
						__ffs(dd->mult_mask);
		dd->last_rounded_n = ((v & dd->div1_mask) >>
						__ffs(dd->div1_mask)) + 1;
	}

	return 0;
}

/**
 * omap3_core_dpll_restore_context - restore the m and n values of the divider
 * @hw: pointer  struct clk_hw
 *
 * Restore the last rounded rate m and n
 * and the enable mask.
 */
void omap3_noncore_dpll_restore_context(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	const struct dpll_data *dd;
	u32 ctrl, mult_div1;

	dd = clk->dpll_data;

	ctrl = ti_clk_ll_ops->clk_readl(&dd->control_reg);
	mult_div1 = ti_clk_ll_ops->clk_readl(&dd->mult_div1_reg);

	if (clk->context == ((ctrl & dd->enable_mask) >>
			     __ffs(dd->enable_mask)) &&
	    dd->last_rounded_m == ((mult_div1 & dd->mult_mask) >>
				   __ffs(dd->mult_mask)) &&
	    dd->last_rounded_n == ((mult_div1 & dd->div1_mask) >>
				   __ffs(dd->div1_mask)) + 1) {
		/* nothing to be done */
		return;
	}

	if (clk->context == DPLL_LOCKED)
		omap3_noncore_dpll_program(clk, 0);
	else
		_omap3_dpll_write_clken(clk, clk->context);
}

/* OMAP3/4 non-CORE DPLL clkops */
const struct clk_hw_omap_ops clkhwops_omap3_dpll = {
	.allow_idle	= omap3_dpll_allow_idle,
	.deny_idle	= omap3_dpll_deny_idle,
};

/**
 * omap3_dpll4_set_rate - set rate for omap3 per-dpll
 * @hw: clock to change
 * @rate: target rate for clock
 * @parent_rate: clock rate of the DPLL parent
 *
 * Check if the current SoC supports the per-dpll reprogram operation
 * or not, and then do the rate change if supported. Returns -EINVAL
 * if not supported, 0 for success, and potential error codes from the
 * clock rate change.
 */
int omap3_dpll4_set_rate(struct clk_hw *hw, unsigned long rate,
			 unsigned long parent_rate)
{
	/*
	 * According to the 12-5 CDP code from TI, "Limitation 2.5"
	 * on 3430ES1 prevents us from changing DPLL multipliers or dividers
	 * on DPLL4.
	 */
	if (ti_clk_get_features()->flags & TI_CLK_DPLL4_DENY_REPROGRAM) {
		pr_err("clock: DPLL4 cannot change rate due to silicon 'Limitation 2.5' on 3430ES1.\n");
		return -EINVAL;
	}

	return omap3_noncore_dpll_set_rate(hw, rate, parent_rate);
}

/**
 * omap3_dpll4_set_rate_and_parent - set rate and parent for omap3 per-dpll
 * @hw: clock to change
 * @rate: target rate for clock
 * @parent_rate: rate of the parent clock
 * @index: parent index, 0 - reference clock, 1 - bypass clock
 *
 * Check if the current SoC support the per-dpll reprogram operation
 * or not, and then do the rate + parent change if supported. Returns
 * -EINVAL if not supported, 0 for success, and potential error codes
 * from the clock rate change.
 */
int omap3_dpll4_set_rate_and_parent(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate, u8 index)
{
	if (ti_clk_get_features()->flags & TI_CLK_DPLL4_DENY_REPROGRAM) {
		pr_err("clock: DPLL4 cannot change rate due to silicon 'Limitation 2.5' on 3430ES1.\n");
		return -EINVAL;
	}

	return omap3_noncore_dpll_set_rate_and_parent(hw, rate, parent_rate,
						      index);
}

/* Apply DM3730 errata sprz319 advisory 2.1. */
static bool omap3_dpll5_apply_errata(struct clk_hw *hw,
				     unsigned long parent_rate)
{
	struct omap3_dpll5_settings {
		unsigned int rate, m, n;
	};

	static const struct omap3_dpll5_settings precomputed[] = {
		/*
		 * From DM3730 errata advisory 2.1, table 35 and 36.
		 * The N value is increased by 1 compared to the tables as the
		 * errata lists register values while last_rounded_field is the
		 * real divider value.
		 */
		{ 12000000,  80,  0 + 1 },
		{ 13000000, 443,  5 + 1 },
		{ 19200000,  50,  0 + 1 },
		{ 26000000, 443, 11 + 1 },
		{ 38400000,  25,  0 + 1 }
	};

	const struct omap3_dpll5_settings *d;
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	struct dpll_data *dd;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(precomputed); ++i) {
		if (parent_rate == precomputed[i].rate)
			break;
	}

	if (i == ARRAY_SIZE(precomputed))
		return false;

	d = &precomputed[i];

	/* Update the M, N and rounded rate values and program the DPLL. */
	dd = clk->dpll_data;
	dd->last_rounded_m = d->m;
	dd->last_rounded_n = d->n;
	dd->last_rounded_rate = div_u64((u64)parent_rate * d->m, d->n);
	omap3_noncore_dpll_program(clk, 0);

	return true;
}

/**
 * omap3_dpll5_set_rate - set rate for omap3 dpll5
 * @hw: clock to change
 * @rate: target rate for clock
 * @parent_rate: rate of the parent clock
 *
 * Set rate for the DPLL5 clock. Apply the sprz319 advisory 2.1 on OMAP36xx if
 * the DPLL is used for USB host (detected through the requested rate).
 */
int omap3_dpll5_set_rate(struct clk_hw *hw, unsigned long rate,
			 unsigned long parent_rate)
{
	if (rate == OMAP3_DPLL5_FREQ_FOR_USBHOST * 8) {
		if (omap3_dpll5_apply_errata(hw, parent_rate))
			return 0;
	}

	return omap3_noncore_dpll_set_rate(hw, rate, parent_rate);
}
