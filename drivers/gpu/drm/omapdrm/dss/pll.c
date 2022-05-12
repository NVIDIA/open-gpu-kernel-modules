// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2014 Texas Instruments Incorporated - https://www.ti.com/
 */

#define DSS_SUBSYS_NAME "PLL"

#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/regulator/consumer.h>
#include <linux/sched.h>

#include "omapdss.h"
#include "dss.h"

#define PLL_CONTROL			0x0000
#define PLL_STATUS			0x0004
#define PLL_GO				0x0008
#define PLL_CONFIGURATION1		0x000C
#define PLL_CONFIGURATION2		0x0010
#define PLL_CONFIGURATION3		0x0014
#define PLL_SSC_CONFIGURATION1		0x0018
#define PLL_SSC_CONFIGURATION2		0x001C
#define PLL_CONFIGURATION4		0x0020

int dss_pll_register(struct dss_device *dss, struct dss_pll *pll)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(dss->plls); ++i) {
		if (!dss->plls[i]) {
			dss->plls[i] = pll;
			pll->dss = dss;
			return 0;
		}
	}

	return -EBUSY;
}

void dss_pll_unregister(struct dss_pll *pll)
{
	struct dss_device *dss = pll->dss;
	int i;

	for (i = 0; i < ARRAY_SIZE(dss->plls); ++i) {
		if (dss->plls[i] == pll) {
			dss->plls[i] = NULL;
			pll->dss = NULL;
			return;
		}
	}
}

struct dss_pll *dss_pll_find(struct dss_device *dss, const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(dss->plls); ++i) {
		if (dss->plls[i] && strcmp(dss->plls[i]->name, name) == 0)
			return dss->plls[i];
	}

	return NULL;
}

struct dss_pll *dss_pll_find_by_src(struct dss_device *dss,
				    enum dss_clk_source src)
{
	struct dss_pll *pll;

	switch (src) {
	default:
	case DSS_CLK_SRC_FCK:
		return NULL;

	case DSS_CLK_SRC_HDMI_PLL:
		return dss_pll_find(dss, "hdmi");

	case DSS_CLK_SRC_PLL1_1:
	case DSS_CLK_SRC_PLL1_2:
	case DSS_CLK_SRC_PLL1_3:
		pll = dss_pll_find(dss, "dsi0");
		if (!pll)
			pll = dss_pll_find(dss, "video0");
		return pll;

	case DSS_CLK_SRC_PLL2_1:
	case DSS_CLK_SRC_PLL2_2:
	case DSS_CLK_SRC_PLL2_3:
		pll = dss_pll_find(dss, "dsi1");
		if (!pll)
			pll = dss_pll_find(dss, "video1");
		return pll;
	}
}

unsigned int dss_pll_get_clkout_idx_for_src(enum dss_clk_source src)
{
	switch (src) {
	case DSS_CLK_SRC_HDMI_PLL:
		return 0;

	case DSS_CLK_SRC_PLL1_1:
	case DSS_CLK_SRC_PLL2_1:
		return 0;

	case DSS_CLK_SRC_PLL1_2:
	case DSS_CLK_SRC_PLL2_2:
		return 1;

	case DSS_CLK_SRC_PLL1_3:
	case DSS_CLK_SRC_PLL2_3:
		return 2;

	default:
		return 0;
	}
}

int dss_pll_enable(struct dss_pll *pll)
{
	int r;

	r = clk_prepare_enable(pll->clkin);
	if (r)
		return r;

	if (pll->regulator) {
		r = regulator_enable(pll->regulator);
		if (r)
			goto err_reg;
	}

	r = pll->ops->enable(pll);
	if (r)
		goto err_enable;

	return 0;

err_enable:
	if (pll->regulator)
		regulator_disable(pll->regulator);
err_reg:
	clk_disable_unprepare(pll->clkin);
	return r;
}

void dss_pll_disable(struct dss_pll *pll)
{
	pll->ops->disable(pll);

	if (pll->regulator)
		regulator_disable(pll->regulator);

	clk_disable_unprepare(pll->clkin);

	memset(&pll->cinfo, 0, sizeof(pll->cinfo));
}

int dss_pll_set_config(struct dss_pll *pll, const struct dss_pll_clock_info *cinfo)
{
	int r;

	r = pll->ops->set_config(pll, cinfo);
	if (r)
		return r;

	pll->cinfo = *cinfo;

	return 0;
}

bool dss_pll_hsdiv_calc_a(const struct dss_pll *pll, unsigned long clkdco,
		unsigned long out_min, unsigned long out_max,
		dss_hsdiv_calc_func func, void *data)
{
	const struct dss_pll_hw *hw = pll->hw;
	int m, m_start, m_stop;
	unsigned long out;

	out_min = out_min ? out_min : 1;
	out_max = out_max ? out_max : ULONG_MAX;

	m_start = max(DIV_ROUND_UP(clkdco, out_max), 1ul);

	m_stop = min((unsigned)(clkdco / out_min), hw->mX_max);

	for (m = m_start; m <= m_stop; ++m) {
		out = clkdco / m;

		if (func(m, out, data))
			return true;
	}

	return false;
}

/*
 * clkdco = clkin / n * m * 2
 * clkoutX = clkdco / mX
 */
bool dss_pll_calc_a(const struct dss_pll *pll, unsigned long clkin,
		unsigned long pll_min, unsigned long pll_max,
		dss_pll_calc_func func, void *data)
{
	const struct dss_pll_hw *hw = pll->hw;
	int n, n_start, n_stop, n_inc;
	int m, m_start, m_stop, m_inc;
	unsigned long fint, clkdco;
	unsigned long pll_hw_max;
	unsigned long fint_hw_min, fint_hw_max;

	pll_hw_max = hw->clkdco_max;

	fint_hw_min = hw->fint_min;
	fint_hw_max = hw->fint_max;

	n_start = max(DIV_ROUND_UP(clkin, fint_hw_max), 1ul);
	n_stop = min((unsigned)(clkin / fint_hw_min), hw->n_max);
	n_inc = 1;

	if (n_start > n_stop)
		return false;

	if (hw->errata_i886) {
		swap(n_start, n_stop);
		n_inc = -1;
	}

	pll_max = pll_max ? pll_max : ULONG_MAX;

	for (n = n_start; n != n_stop; n += n_inc) {
		fint = clkin / n;

		m_start = max(DIV_ROUND_UP(DIV_ROUND_UP(pll_min, fint), 2),
				1ul);
		m_stop = min3((unsigned)(pll_max / fint / 2),
				(unsigned)(pll_hw_max / fint / 2),
				hw->m_max);
		m_inc = 1;

		if (m_start > m_stop)
			continue;

		if (hw->errata_i886) {
			swap(m_start, m_stop);
			m_inc = -1;
		}

		for (m = m_start; m != m_stop; m += m_inc) {
			clkdco = 2 * m * fint;

			if (func(n, m, fint, clkdco, data))
				return true;
		}
	}

	return false;
}

/*
 * This calculates a PLL config that will provide the target_clkout rate
 * for clkout. Additionally clkdco rate will be the same as clkout rate
 * when clkout rate is >= min_clkdco.
 *
 * clkdco = clkin / n * m + clkin / n * mf / 262144
 * clkout = clkdco / m2
 */
bool dss_pll_calc_b(const struct dss_pll *pll, unsigned long clkin,
	unsigned long target_clkout, struct dss_pll_clock_info *cinfo)
{
	unsigned long fint, clkdco, clkout;
	unsigned long target_clkdco;
	unsigned long min_dco;
	unsigned int n, m, mf, m2, sd;
	const struct dss_pll_hw *hw = pll->hw;

	DSSDBG("clkin %lu, target clkout %lu\n", clkin, target_clkout);

	/* Fint */
	n = DIV_ROUND_UP(clkin, hw->fint_max);
	fint = clkin / n;

	/* adjust m2 so that the clkdco will be high enough */
	min_dco = roundup(hw->clkdco_min, fint);
	m2 = DIV_ROUND_UP(min_dco, target_clkout);
	if (m2 == 0)
		m2 = 1;

	target_clkdco = target_clkout * m2;
	m = target_clkdco / fint;

	clkdco = fint * m;

	/* adjust clkdco with fractional mf */
	if (WARN_ON(target_clkdco - clkdco > fint))
		mf = 0;
	else
		mf = (u32)div_u64(262144ull * (target_clkdco - clkdco), fint);

	if (mf > 0)
		clkdco += (u32)div_u64((u64)mf * fint, 262144);

	clkout = clkdco / m2;

	/* sigma-delta */
	sd = DIV_ROUND_UP(fint * m, 250000000);

	DSSDBG("N = %u, M = %u, M.f = %u, M2 = %u, SD = %u\n",
		n, m, mf, m2, sd);
	DSSDBG("Fint %lu, clkdco %lu, clkout %lu\n", fint, clkdco, clkout);

	cinfo->n = n;
	cinfo->m = m;
	cinfo->mf = mf;
	cinfo->mX[0] = m2;
	cinfo->sd = sd;

	cinfo->fint = fint;
	cinfo->clkdco = clkdco;
	cinfo->clkout[0] = clkout;

	return true;
}

static int wait_for_bit_change(void __iomem *reg, int bitnum, int value)
{
	unsigned long timeout;
	ktime_t wait;
	int t;

	/* first busyloop to see if the bit changes right away */
	t = 100;
	while (t-- > 0) {
		if (FLD_GET(readl_relaxed(reg), bitnum, bitnum) == value)
			return value;
	}

	/* then loop for 500ms, sleeping for 1ms in between */
	timeout = jiffies + msecs_to_jiffies(500);
	while (time_before(jiffies, timeout)) {
		if (FLD_GET(readl_relaxed(reg), bitnum, bitnum) == value)
			return value;

		wait = ns_to_ktime(1000 * 1000);
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_hrtimeout(&wait, HRTIMER_MODE_REL);
	}

	return !value;
}

int dss_pll_wait_reset_done(struct dss_pll *pll)
{
	void __iomem *base = pll->base;

	if (wait_for_bit_change(base + PLL_STATUS, 0, 1) != 1)
		return -ETIMEDOUT;
	else
		return 0;
}

static int dss_wait_hsdiv_ack(struct dss_pll *pll, u32 hsdiv_ack_mask)
{
	int t = 100;

	while (t-- > 0) {
		u32 v = readl_relaxed(pll->base + PLL_STATUS);
		v &= hsdiv_ack_mask;
		if (v == hsdiv_ack_mask)
			return 0;
	}

	return -ETIMEDOUT;
}

static bool pll_is_locked(u32 stat)
{
	/*
	 * Required value for each bitfield listed below
	 *
	 * PLL_STATUS[6] = 0  PLL_BYPASS
	 * PLL_STATUS[5] = 0  PLL_HIGHJITTER
	 *
	 * PLL_STATUS[3] = 0  PLL_LOSSREF
	 * PLL_STATUS[2] = 0  PLL_RECAL
	 * PLL_STATUS[1] = 1  PLL_LOCK
	 * PLL_STATUS[0] = 1  PLL_CTRL_RESET_DONE
	 */
	return ((stat & 0x6f) == 0x3);
}

int dss_pll_write_config_type_a(struct dss_pll *pll,
		const struct dss_pll_clock_info *cinfo)
{
	const struct dss_pll_hw *hw = pll->hw;
	void __iomem *base = pll->base;
	int r = 0;
	u32 l;

	l = 0;
	if (hw->has_stopmode)
		l = FLD_MOD(l, 1, 0, 0);		/* PLL_STOPMODE */
	l = FLD_MOD(l, cinfo->n - 1, hw->n_msb, hw->n_lsb);	/* PLL_REGN */
	l = FLD_MOD(l, cinfo->m, hw->m_msb, hw->m_lsb);		/* PLL_REGM */
	/* M4 */
	l = FLD_MOD(l, cinfo->mX[0] ? cinfo->mX[0] - 1 : 0,
			hw->mX_msb[0], hw->mX_lsb[0]);
	/* M5 */
	l = FLD_MOD(l, cinfo->mX[1] ? cinfo->mX[1] - 1 : 0,
			hw->mX_msb[1], hw->mX_lsb[1]);
	writel_relaxed(l, base + PLL_CONFIGURATION1);

	l = 0;
	/* M6 */
	l = FLD_MOD(l, cinfo->mX[2] ? cinfo->mX[2] - 1 : 0,
			hw->mX_msb[2], hw->mX_lsb[2]);
	/* M7 */
	l = FLD_MOD(l, cinfo->mX[3] ? cinfo->mX[3] - 1 : 0,
			hw->mX_msb[3], hw->mX_lsb[3]);
	writel_relaxed(l, base + PLL_CONFIGURATION3);

	l = readl_relaxed(base + PLL_CONFIGURATION2);
	if (hw->has_freqsel) {
		u32 f = cinfo->fint < 1000000 ? 0x3 :
			cinfo->fint < 1250000 ? 0x4 :
			cinfo->fint < 1500000 ? 0x5 :
			cinfo->fint < 1750000 ? 0x6 :
			0x7;

		l = FLD_MOD(l, f, 4, 1);	/* PLL_FREQSEL */
	} else if (hw->has_selfreqdco) {
		u32 f = cinfo->clkdco < hw->clkdco_low ? 0x2 : 0x4;

		l = FLD_MOD(l, f, 3, 1);	/* PLL_SELFREQDCO */
	}
	l = FLD_MOD(l, 1, 13, 13);		/* PLL_REFEN */
	l = FLD_MOD(l, 0, 14, 14);		/* PHY_CLKINEN */
	l = FLD_MOD(l, 0, 16, 16);		/* M4_CLOCK_EN */
	l = FLD_MOD(l, 0, 18, 18);		/* M5_CLOCK_EN */
	l = FLD_MOD(l, 1, 20, 20);		/* HSDIVBYPASS */
	if (hw->has_refsel)
		l = FLD_MOD(l, 3, 22, 21);	/* REFSEL = sysclk */
	l = FLD_MOD(l, 0, 23, 23);		/* M6_CLOCK_EN */
	l = FLD_MOD(l, 0, 25, 25);		/* M7_CLOCK_EN */
	writel_relaxed(l, base + PLL_CONFIGURATION2);

	if (hw->errata_i932) {
		int cnt = 0;
		u32 sleep_time;
		const u32 max_lock_retries = 20;

		/*
		 * Calculate wait time for PLL LOCK
		 * 1000 REFCLK cycles in us.
		 */
		sleep_time = DIV_ROUND_UP(1000*1000*1000, cinfo->fint);

		for (cnt = 0; cnt < max_lock_retries; cnt++) {
			writel_relaxed(1, base + PLL_GO);	/* PLL_GO */

			/**
			 * read the register back to ensure the write is
			 * flushed
			 */
			readl_relaxed(base + PLL_GO);

			usleep_range(sleep_time, sleep_time + 5);
			l = readl_relaxed(base + PLL_STATUS);

			if (pll_is_locked(l) &&
			    !(readl_relaxed(base + PLL_GO) & 0x1))
				break;

		}

		if (cnt == max_lock_retries) {
			DSSERR("cannot lock PLL\n");
			r = -EIO;
			goto err;
		}
	} else {
		writel_relaxed(1, base + PLL_GO);	/* PLL_GO */

		if (wait_for_bit_change(base + PLL_GO, 0, 0) != 0) {
			DSSERR("DSS DPLL GO bit not going down.\n");
			r = -EIO;
			goto err;
		}

		if (wait_for_bit_change(base + PLL_STATUS, 1, 1) != 1) {
			DSSERR("cannot lock DSS DPLL\n");
			r = -EIO;
			goto err;
		}
	}

	l = readl_relaxed(base + PLL_CONFIGURATION2);
	l = FLD_MOD(l, 1, 14, 14);			/* PHY_CLKINEN */
	l = FLD_MOD(l, cinfo->mX[0] ? 1 : 0, 16, 16);	/* M4_CLOCK_EN */
	l = FLD_MOD(l, cinfo->mX[1] ? 1 : 0, 18, 18);	/* M5_CLOCK_EN */
	l = FLD_MOD(l, 0, 20, 20);			/* HSDIVBYPASS */
	l = FLD_MOD(l, cinfo->mX[2] ? 1 : 0, 23, 23);	/* M6_CLOCK_EN */
	l = FLD_MOD(l, cinfo->mX[3] ? 1 : 0, 25, 25);	/* M7_CLOCK_EN */
	writel_relaxed(l, base + PLL_CONFIGURATION2);

	r = dss_wait_hsdiv_ack(pll,
		(cinfo->mX[0] ? BIT(7) : 0) |
		(cinfo->mX[1] ? BIT(8) : 0) |
		(cinfo->mX[2] ? BIT(10) : 0) |
		(cinfo->mX[3] ? BIT(11) : 0));
	if (r) {
		DSSERR("failed to enable HSDIV clocks\n");
		goto err;
	}

err:
	return r;
}

int dss_pll_write_config_type_b(struct dss_pll *pll,
		const struct dss_pll_clock_info *cinfo)
{
	const struct dss_pll_hw *hw = pll->hw;
	void __iomem *base = pll->base;
	u32 l;

	l = 0;
	l = FLD_MOD(l, cinfo->m, 20, 9);	/* PLL_REGM */
	l = FLD_MOD(l, cinfo->n - 1, 8, 1);	/* PLL_REGN */
	writel_relaxed(l, base + PLL_CONFIGURATION1);

	l = readl_relaxed(base + PLL_CONFIGURATION2);
	l = FLD_MOD(l, 0x0, 12, 12);	/* PLL_HIGHFREQ divide by 2 */
	l = FLD_MOD(l, 0x1, 13, 13);	/* PLL_REFEN */
	l = FLD_MOD(l, 0x0, 14, 14);	/* PHY_CLKINEN */
	if (hw->has_refsel)
		l = FLD_MOD(l, 0x3, 22, 21);	/* REFSEL = SYSCLK */

	/* PLL_SELFREQDCO */
	if (cinfo->clkdco > hw->clkdco_low)
		l = FLD_MOD(l, 0x4, 3, 1);
	else
		l = FLD_MOD(l, 0x2, 3, 1);
	writel_relaxed(l, base + PLL_CONFIGURATION2);

	l = readl_relaxed(base + PLL_CONFIGURATION3);
	l = FLD_MOD(l, cinfo->sd, 17, 10);	/* PLL_REGSD */
	writel_relaxed(l, base + PLL_CONFIGURATION3);

	l = readl_relaxed(base + PLL_CONFIGURATION4);
	l = FLD_MOD(l, cinfo->mX[0], 24, 18);	/* PLL_REGM2 */
	l = FLD_MOD(l, cinfo->mf, 17, 0);	/* PLL_REGM_F */
	writel_relaxed(l, base + PLL_CONFIGURATION4);

	writel_relaxed(1, base + PLL_GO);	/* PLL_GO */

	if (wait_for_bit_change(base + PLL_GO, 0, 0) != 0) {
		DSSERR("DSS DPLL GO bit not going down.\n");
		return -EIO;
	}

	if (wait_for_bit_change(base + PLL_STATUS, 1, 1) != 1) {
		DSSERR("cannot lock DSS DPLL\n");
		return -ETIMEDOUT;
	}

	return 0;
}
